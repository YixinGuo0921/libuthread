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

queue_t q;

struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
};

struct uthread_tcb *uthread_current(void)
{
        /* TODO Phase 2/4 */
        //TEMPORARY TO STOP ERRORS
        struct uthread_tcb* tcb = malloc(sizeof(struct uthread_tcb));
        return tcb;
}

void uthread_yield(void)
{
        
}

void uthread_exit(void)
{
        /* TODO Phase 2 */
}

int uthread_create(uthread_func_t func, void *arg)
{
        // Allocate space for TCB, context, and stack pointer.
        struct uthread_tcb* tcb = malloc(sizeof(struct uthread_tcb));
        uthread_ctx_t* thread_ctx = malloc(sizeof(uthread_ctx_t));
        void* stack_ptr = uthread_ctx_alloc_stack();

        if (tcb == NULL || thread_ctx == NULL || stack_ptr == NULL) return -1; // malloc failed

        if (uthread_ctx_init(thread_ctx, stack_ptr, func, arg) != 0) return -1;
        
        tcb->thread_ctx = thread_ctx;
        tcb->stack_ptr = stack_ptr;
        queue_enqueue(q, tcb);

        return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
        // Thread queue
        q = queue_create();

        // Create idle & initial contexts
        uthread_ctx_t* idle_ctx = malloc(sizeof(uthread_ctx_t));
        struct uthread_tcb* current_tcb = malloc(sizeof(struct uthread_tcb));

        // Create initial thread
        if (uthread_create(func, arg) != 0) return -1;

        // Use preempt (TEMPORARY)
        if (preempt) return -1;

        while (queue_length(q) != 0) {
                queue_dequeue(q, (void**) & current_tcb);
                uthread_ctx_switch(idle_ctx, current_tcb->thread_ctx); //does not destroy stacks yet
        }

        uthread_ctx_switch(current_tcb->thread_ctx, idle_ctx);

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

