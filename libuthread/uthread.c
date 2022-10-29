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

#define  READY 0
#define  RUNNING 1
#define  BLOCKED 2
#define  EXITED 3

static struct uthread_tcb* running_tcb;
static queue_t thread_queue;
uthread_ctx_t* idle_ctx;

/* TCB Data Structure */
struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
        int state;
};

/* Queue Functions */
static void set_running_thread(queue_t q, void* data)
{
        if (queue_length(q) == 0)
                return;

        struct uthread_tcb* tcb_tmp = data;
        if (tcb_tmp->state == RUNNING) {
                running_tcb = tcb_tmp;
        }
}

void debug_print_queue(queue_t q, void* data)
{
        if (queue_length(q) == 0)
                return;

        struct uthread_tcb* tcb_tmp = data;

        printf("QL%d: %p, State: %d\n", queue_length(thread_queue), tcb_tmp->stack_ptr, tcb_tmp->state);
}

/* Thread Header Functions */
struct uthread_tcb *uthread_current(void)
{
        // Sets running_tcb to the running thread
        if (running_tcb->state != RUNNING)
                queue_iterate(thread_queue, set_running_thread);

        return running_tcb;
}

void uthread_yield(void)
{
        // Safeguard from interruption
        struct uthread_tcb* current_tcb = uthread_current();

        // Delete this thread from thread queue
        queue_delete(thread_queue, current_tcb);

        // Queue this thread to the back (context not updated yet)
        queue_enqueue(thread_queue, current_tcb);

        // Switch to idle (updates running context)
        current_tcb->state = READY;
        uthread_ctx_switch(current_tcb->thread_ctx, idle_ctx);
        current_tcb->state = RUNNING;
}

void uthread_exit(void)
{
        // Safeguard from interruption
        struct uthread_tcb* current_tcb = uthread_current();

        // Delete this thread from thread queue
        queue_delete(thread_queue, current_tcb);

        // Free this thread (unnecessary?)
        uthread_ctx_destroy_stack(current_tcb->stack_ptr);
        current_tcb->state = EXITED;
        free(current_tcb);

        // Switch back to idle (main) context w/o saving
        setcontext(idle_ctx);

        queue_iterate(thread_queue, debug_print_queue); //This should never run
}

int uthread_create(uthread_func_t func, void *arg)
{
        // Allocate space for new context and stack pointer.
        struct uthread_tcb* new_tcb = malloc(sizeof(struct uthread_tcb));
        uthread_ctx_t* thread_ctx = malloc(sizeof(uthread_ctx_t));
        void* stack_ptr = uthread_ctx_alloc_stack();

        if (new_tcb == NULL || thread_ctx == NULL || stack_ptr == NULL) return -1; // malloc failed

        if (uthread_ctx_init(thread_ctx, stack_ptr, func, arg) != 0) return -1;
        
        new_tcb->thread_ctx = thread_ctx;
        new_tcb->stack_ptr = stack_ptr;
        new_tcb->state = READY;
        queue_enqueue(thread_queue, new_tcb);
        return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
        /*Use preempt var to avoid gcc error(TEMPORARY)*/
        if (preempt) return -1;

        // Create FIFO queue, will ALWAYS hold threads unless exited
        thread_queue = queue_create();

        // Initialize Idle & Initial Threads
        idle_ctx = malloc(sizeof(uthread_ctx_t));
        struct uthread_tcb* initial_tcb = malloc(sizeof(struct uthread_tcb));

        // Initialize running thread
        running_tcb = initial_tcb;

        // Create initial TCB
        if (uthread_create(func, arg) != 0) return -1;

        do {
                //Get next thread and queue it to the back (functionally the same as just reading the first element)
                do {
                        queue_dequeue(thread_queue, (void**)&initial_tcb);
                        queue_enqueue(thread_queue, initial_tcb);

                } while (initial_tcb->state == BLOCKED); // state is RUNNING


                initial_tcb->state = RUNNING;
                uthread_ctx_switch(idle_ctx, initial_tcb->thread_ctx);

        } while (queue_length(thread_queue) != 0);

        return 0;
}

void uthread_block(void)
{
        /* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
        //TEMPORARY to stop gcc errors
        (void)uthread;

        /* TODO Phase 4 */
}

