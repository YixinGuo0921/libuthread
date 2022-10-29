#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

#define  READY 0
#define  RUNNING 1
#define  BLOCKED 2
#define  EXITED 3

extern uthread_ctx_t* idle_ctx;

struct semaphore { //sem_t is a pointer to a struct semaphore
        queue_t waiting_room;
        unsigned int resource;
};

struct uthread_tcb {
        uthread_ctx_t* thread_ctx;
        void* stack_ptr;
        int state;
};

sem_t sem_create(size_t count)
{
        sem_t new_sem = malloc(sizeof(struct semaphore));
        new_sem->waiting_room = queue_create();

        new_sem->resource = count;
        return new_sem;
}

int sem_destroy(sem_t sem)
{
        free(sem);
        return 0;
}

int sem_down(sem_t sem)
{
        // Get caller thread
        struct uthread_tcb* caller_tcb = uthread_current();

        // If resources are available, take one and continue run
        if (sem->resource != 0) {
                sem->resource--;
                return 0;
        }

        /* NO RESOURCES AVAILABLE */

        // Keep record of threads waiting for resource (FIFO)
        queue_enqueue(sem->waiting_room, caller_tcb);

        // block caller thread and go to next thread in thread_queue
        caller_tcb->state = BLOCKED;
        uthread_ctx_switch(caller_tcb->thread_ctx, idle_ctx);

        return -1;
}

int sem_up(sem_t sem)
{
        struct uthread_tcb* blocked_tcb;

        // If no one in waiting room, continue thread
        if (queue_length(sem->waiting_room) == 0)
        {
                sem->resource++;
                return 0;
        }

        // Otherwise, dequeue from waiting room and make thread available for queue again
        queue_dequeue(sem->waiting_room, (void**)&blocked_tcb);
        blocked_tcb->state = READY;

        return 0;
}

