/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 *	thread3
 *	thread2
 *	thread1
 *	Returned!
 * 
 * If yield doesn't work, the order will be
 * 
 *	thread1
 *	thread2
 *	thread3
 *	Returned!
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;

	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_yield();
	uthread_yield();
	printf("thread1\n");
}

int main(void)
{
	uthread_run(false, thread1, NULL);
	printf("Returned!\n");
	return 0;
}
