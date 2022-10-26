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

struct uthread_tcb* initial_tcb;
uthread_ctx_t* idle_ctx;
queue_t thread_queue;

struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
        int state;
};

struct uthread_tcb *uthread_current(void)
{
        /* TODO Phase 4 */

        return initial_tcb;
}

void uthread_yield(void)
{

}

void uthread_exit(void)
{
        // Free current thread
        uthread_ctx_destroy_stack(initial_tcb->stack_ptr);
        free(initial_tcb);

        // Switch back to idle (main) context
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

        do {
                queue_dequeue(thread_queue, (void**)&initial_tcb);
                uthread_ctx_switch(idle_ctx, initial_tcb->thread_ctx); //calls uthread_exit() when return
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

