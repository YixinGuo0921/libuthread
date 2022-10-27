#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

queue_t waiting_queue;

struct semaphore { //sem_t is a pointer to a struct semaphore
	int resource;
};

sem_t sem_create(size_t count)
{
	sem_t new_semaphore = malloc(sizeof(struct semaphore));
	waiting_queue = queue_create();

	new_semaphore->resource = count;
	return new_semaphore;
}

int sem_destroy(sem_t sem)
{
	free(sem);
	free(waiting_queue);

	return 0;
}

int sem_down(sem_t sem)
{
	if (sem->resource == 0)
	{
		queue_enqueue(waiting_queue, uthread_current());
		uthread_block();
		return -1;
	}

	sem->resource--;
	return 0;
}

int sem_up(sem_t sem)
{
	sem->resource++;

	return 0;
}

