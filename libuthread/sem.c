#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // for printf debug

#include "private.h"
#include "queue.h"
#include "sem.h"

#define  READY          0 // Threads waiting for their turn
#define  RUNNING        1 // The current thread
#define  BLOCKED        2 // Threads that are semaphore blocked
#define  UNBLOCKED      3 // Like READY, but was previously waiting for a semaphore
#define  EXITED         4 // Threads that have fully completed (unused conditionally)

// From uthread.c
/*
* @waiting_room: All the threads that are waiting for this semaphore's resource
* @released_threads: For corner case; contains all the threads that were initially blocked via semaphore. Resets whenever a resource is taken normally
* @resource: the count of resources a semaphore has
*/
struct semaphore {
	queue_t released_threads;
	queue_t waiting_room;
	unsigned int resource;
}; 
struct uthread_tcb {			// For handle_unblocked() to see states
	uthread_ctx_t* thread_ctx;
	void* stack_ptr;
	int state;
};

/* queue_iterate() callback functions*/
void empty_queue(queue_t q, void* data)
{
	queue_dequeue(q, &data);
}

/* Semaphore Functions*/
sem_t sem_create(size_t count)
{
	sem_t new_sem = malloc(sizeof(struct semaphore));

	if (new_sem == NULL)
		return NULL;

	new_sem->waiting_room = queue_create();
	new_sem->released_threads = queue_create();

	new_sem->resource = count;
	return new_sem;
}

int sem_destroy(sem_t sem)
{
	preempt_disable();
	if (queue_destroy(sem->waiting_room) != 0)
		return -1; //threads still being blocked

	// Empty released_threads before destroying it
	queue_iterate(sem->released_threads, empty_queue);
	queue_destroy(sem->released_threads);

	preempt_enable();
	free(sem);

	return 0;
}

/* 
* Changes all UNBLOCKED threads BLOCKED and transfers them to waiting_room.
* (This is for corner case)
*/
void handle_unblocked(sem_t sem)
{
	struct uthread_tcb* tcb_address;

	while (queue_length(sem->released_threads) != 0) {

		queue_dequeue(sem->released_threads, (void**)&tcb_address);

		// UNBLOCKED iff sem_up specifically released it
		if (tcb_address->state != UNBLOCKED)
			continue;

		tcb_address->state = BLOCKED;
		queue_enqueue(sem->waiting_room, tcb_address);
	}
}

int sem_down(sem_t sem)
{
	if (sem == NULL)
		return -1;

	preempt_disable();

	// Get caller thread
	struct uthread_tcb* caller_tcb = uthread_current();

	// If resources are available, check if any threads were waiting for that resource (corner case protection)
	if (sem->resource > 0) {
		handle_unblocked(sem);
	}
	// Otherwise, keep record of threads waiting for resource (FIFO) and block
	else {
		queue_enqueue(sem->waiting_room, caller_tcb);
		uthread_block();
	}

	preempt_enable();
	sem->resource--;
	return 0;
}

int sem_up(sem_t sem)
{
	if (sem == NULL)
		return -1;

	struct uthread_tcb* blocked_tcb;

	sem->resource++;

	// If no one in waiting room, continue thread
	if (queue_length(sem->waiting_room) == 0)
		return 0;

	preempt_disable();

	// Otherwise, dequeue from waiting room and make thread available for queue again
	queue_dequeue(sem->waiting_room, (void**)&blocked_tcb);
	queue_enqueue(sem->released_threads, blocked_tcb); // track released thread

	uthread_unblock(blocked_tcb);

	preempt_enable();

	return 0;
}

