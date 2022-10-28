#include <stddef.h>
#include <stdlib.h>
#include <stdio.h> // for printf

#include "queue.h"
#include "sem.h"
#include "private.h"


struct semaphore { //sem_t is a pointer to a struct semaphore
	queue_t waiting_room;
	unsigned int resource;
};

sem_t sem_create(size_t count)
{
	sem_t new_semaphore = malloc(sizeof(struct semaphore));
	new_semaphore->waiting_room = queue_create();

	new_semaphore->resource = count;
	return new_semaphore;
}

int sem_destroy(sem_t sem)
{
	free(sem);
	free(sem->waiting_room);

	return 0;
}

int sem_down(sem_t sem)
{
	// Get caller thread
	struct uthread_tcb* caller_tcb = uthread_current();

	printf("Running sem_down...\n");

	// If resources AREN'T available, block caller thread and go to next IN THREAD_QUEUE
	if (sem->resource == 0) {
		queue_enqueue(sem->waiting_room, caller_tcb);
		printf("BLOCKING %p\n", caller_tcb);
		uthread_block();
		return -1;
	}

	// If there are resources available, take one and continue run
	sem->resource--;
	return 0;
}

int sem_up(sem_t sem)
{
	struct uthread_tcb* blocked_tcb;

	printf("Running sem_up...\n");

	sem->resource++;

	if (queue_length(sem->waiting_room) == 0)
		return 0;

	queue_dequeue(sem->waiting_room, (void**)&blocked_tcb);
	printf("[UN]BLOCKING %p\n", blocked_tcb);
	uthread_unblock(blocked_tcb);
	sem->resource--;

	return 0;
}

