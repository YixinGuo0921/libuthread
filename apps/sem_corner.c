/*
 * Semaphore corner case test
 *
 * Should forever wait after printing
 * 
 *         thread2
 *         thread3
 * 
 * If the output is
 * 
 *         thread2
 *         thread3
 *         thread1
 * 
 * then corner case protection did not work. 
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include <sem.h>
#include <uthread.h>

int total;
sem_t sem;

static void thread3(void *arg)
{
	(void)arg;

	sem_down(sem);		/* Steal resource from thread1 */
	printf("thread3\n");
}

static void thread2(void *arg)
{
	(void)arg;

	sem_up(sem);		/* Release thread1 */
	printf("thread2\n");
}

static void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_create(thread3, NULL);

	sem_down(sem);		/* Block thread1 */
	printf("thread1\n");
}

int main(void)
{
	sem = sem_create(0);
	total = 0;

	uthread_run(false, thread1, NULL);

	if(sem_destroy(sem) != 0) printf("FAILURE\n");

	return 0;
}
