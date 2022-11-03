#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

#define UNUSED(x) (void)(x)

#define  READY          0 // Threads waiting for their turn
#define  RUNNING        1 // The current thread
#define  BLOCKED        2 // Threads that are semaphore blocked
#define  UNBLOCKED      3 // Like READY, but was previously waiting for a semaphore
#define  EXITED         4 // Threads that have fully completed (unused conditionally)

static struct uthread_tcb* running_tcb;
queue_t thread_queue;
uthread_ctx_t* idle_ctx;

/*
* @thread_ctx: The current context of a thread (i.e. its PC and associated function)
* @stack_ptr: pointer to the thread's stack
* @state: The tracker for which state the thread is in (acts as extra context)
*/
struct uthread_tcb {
	uthread_ctx_t* thread_ctx;
	void* stack_ptr;
	int state;
};

//* queue_iterate() callback functions */
static void set_running_thread(queue_t q, void* data)
{
	if (queue_length(q) == 0)
		return;

	struct uthread_tcb* tcb_tmp = data;
	if (tcb_tmp->state == RUNNING) {
		running_tcb = tcb_tmp;
	}
}

void handle_unblocked(queue_t q, void* data)
{
	if (queue_length(q) == 0)
		return;

	struct uthread_tcb* tcb_address = (struct uthread_tcb*)data;

	if (tcb_address->state == UNBLOCKED) // UNBLOCKED iff sem_up specifically released it
		tcb_address->state = BLOCKED;

	queue_dequeue(q, &data);
}

/* uthread Functions */
struct uthread_tcb* uthread_current(void)
{
	// Sets running_tcb to the running thread, if not handled by idle loop (never executes in testing)
	if (running_tcb->state != RUNNING) {
		preempt_disable();
		queue_iterate(thread_queue, set_running_thread);
		preempt_enable();
	}

	return running_tcb;
}

void uthread_yield(void)
{
	struct uthread_tcb* current_tcb = uthread_current();

	preempt_disable();

	// Queue to back and change state
	queue_delete(thread_queue, current_tcb);
	queue_enqueue(thread_queue, current_tcb);
	current_tcb->state = READY;

	preempt_enable();

	uthread_ctx_switch(current_tcb->thread_ctx, idle_ctx);
}

void uthread_exit(void)
{
	struct uthread_tcb* current_tcb = uthread_current();

	preempt_disable();

	// Delete this thread permanently
	queue_delete(thread_queue, current_tcb);

	preempt_enable();

	uthread_ctx_destroy_stack(current_tcb->stack_ptr);
	current_tcb->state = EXITED; // unnecessary?
	free(current_tcb);

	// Switch back to idle (main) context w/o saving
	setcontext(idle_ctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
	// Allocate space for new context and stack pointer.
	struct uthread_tcb* new_tcb = malloc(sizeof(struct uthread_tcb));
	uthread_ctx_t* thread_ctx = malloc(sizeof(uthread_ctx_t));
	void* stack_ptr = uthread_ctx_alloc_stack();

	if (new_tcb == NULL || thread_ctx == NULL || stack_ptr == NULL) // malloc failed
		return -1; 

	/*Begin initialization*/
	preempt_disable();

	if (uthread_ctx_init(thread_ctx, stack_ptr, func, arg) != 0)
		return -1;
	
	new_tcb->thread_ctx = thread_ctx;
	new_tcb->stack_ptr = stack_ptr;
	new_tcb->state = READY;

	queue_enqueue(thread_queue, new_tcb);

	preempt_enable();

	return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	preempt_start(preempt);

	// thread_queue will ALWAYS hold threads unless exited
	thread_queue = queue_create();

	// Initialize Idle & Initial Threads
	idle_ctx = malloc(sizeof(uthread_ctx_t));
	struct uthread_tcb* initial_tcb = malloc(sizeof(struct uthread_tcb));

	if (idle_ctx == NULL || initial_tcb == NULL) // malloc failed
		return -1;

	// Create initial TCB
	if (uthread_create(func, arg) != 0)
		return -1;

	while (queue_length(thread_queue) != 0) {

		preempt_disable();

		/* Choose next thread */
		queue_dequeue(thread_queue, (void**)&initial_tcb);
		queue_enqueue(thread_queue, initial_tcb);

		if (initial_tcb->state == BLOCKED) // re-choose if blocked
			continue;

		initial_tcb->state = RUNNING;
		running_tcb = initial_tcb;

		preempt_enable();

		uthread_ctx_switch(idle_ctx, initial_tcb->thread_ctx); // reassign idle ctx every loop
	} 

	// Clean-up
	if(preempt)
		preempt_stop();

	queue_destroy(thread_queue);

	return 0;
}

/* For Semaphore usage */
void uthread_block(void)
{
	struct uthread_tcb* current_tcb = uthread_current();
	current_tcb->state = BLOCKED;
	uthread_ctx_switch(current_tcb->thread_ctx, idle_ctx);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = UNBLOCKED;
}

