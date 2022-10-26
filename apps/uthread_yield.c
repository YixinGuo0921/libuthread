/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;

	printf("TEST: YIELDING 3\n");
	uthread_yield();
	printf("RE-RUNNING 3\n");

	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);

	printf("TEST: YIELDING 2\n");
	uthread_yield();
	printf("RE-RUNNING 2\n");

	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);

	printf("TEST: YIELDING 1\n");
	uthread_yield();
	printf("RE-RUNNING 1\n");

	printf("thread1\n");

	printf("TEST: YIELDING 1\n");
	uthread_yield();
	printf("RE-RUNNING 1\n");
}

int main(void)
{
	uthread_run(false, thread1, NULL);
	return 0;
}
