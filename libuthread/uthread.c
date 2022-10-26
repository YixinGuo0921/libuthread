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
#define  EXITED 2

struct uthread_tcb* running_tcb;
struct uthread_tcb* initial_tcb;
uthread_ctx_t* idle_ctx;
queue_t thread_queue;



/*TCB Data Structure*/
struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
        int state;
};

/*Queue Functions*/
static void set_first_in_queue(queue_t q, void* data)
{
        if (queue_length(q) == 0)
                return;

        struct uthread_tcb* tcb_tmp = data;
        if (tcb_tmp->state == READY) {
                initial_tcb = tcb_tmp;
        }
}

static void set_running(queue_t q, void* data)
{
        if (queue_length(q) == 0)
                return;

        struct uthread_tcb* tcb_tmp = data;
        if (tcb_tmp->state == RUNNING) {
                running_tcb = tcb_tmp;
        }
}

static void debug_print_queue(queue_t q, void* data)
{
        if (queue_length(q) == 0)
                return;

        struct uthread_tcb* tcb_tmp = data;

        printf("QL%d: %p, State: %d\n", queue_length(thread_queue), tcb_tmp->stack_ptr, tcb_tmp->state);
}

/* Thread Functions*/
struct uthread_tcb *uthread_current(void)
{
        queue_iterate(thread_queue, set_running); // Sets running_tcb to the running thread

        return running_tcb;
}

void uthread_yield(void)
{
        uthread_current();

        queue_iterate(thread_queue, debug_print_queue);

        queue_delete(thread_queue, running_tcb);        // Delete this thread from thread queue

        queue_iterate(thread_queue, debug_print_queue);

        running_tcb->state = READY;                     // Update this thread's state
        queue_enqueue(thread_queue, running_tcb);       // Queue this thread to the back (context not updated yet)
        
        queue_iterate(thread_queue, debug_print_queue);
        printf("\n");

        uthread_ctx_switch(running_tcb->thread_ctx, idle_ctx);      // Switch to idle (updates running context)
}

void uthread_exit(void)
{ 
        // Delete this thread from thread queue
        queue_delete(thread_queue, uthread_current());

        // Free this thread
        uthread_ctx_destroy_stack(running_tcb->stack_ptr);
        free(running_tcb);

        // Switch back to idle (main) context w/o saving
        setcontext(idle_ctx);
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

        // Create FIFO queue
        thread_queue = queue_create();

        // Initialize Idle & Initial Threads
        idle_ctx = malloc(sizeof(uthread_ctx_t));
        initial_tcb = malloc(sizeof(struct uthread_tcb));

        // Create initial TCB
        if (uthread_create(func, arg) != 0) return -1;

        queue_iterate(thread_queue, set_first_in_queue); // Sets initial_tcb

        do {
                initial_tcb->state = RUNNING;
                uthread_ctx_switch(idle_ctx, initial_tcb->thread_ctx);
                //queue_iterate(thread_queue, set_first_in_queue); // Sets initial_tcb

                if (queue_length(thread_queue) == 0) break;

                //If queue not empty, get next thread and queue it to the back
                queue_dequeue(thread_queue, (void**)&initial_tcb);
                queue_enqueue(thread_queue, initial_tcb);

        } while (queue_length(thread_queue) != 0);

        return 0;
}

void uthread_block(void)
{
        /* TODO Phase 4 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
        /* TODO Phase 4 */

        //TEMPORARY TO STOP ERRORS
        if (uthread) return;
}

