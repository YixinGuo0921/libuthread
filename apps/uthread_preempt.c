/*
 * Thread preemption test
 *
 * Tests the preemption feature of our multi-threading library. If preemption does not work, the
 * terminal will only read
 *
 *	I am a hog!
 *	I am a hog!
 *	I am a hog!
 *	...
 * 
 * If it does, then the terminal will read
 *
 *	I am a hog!
 *	I am a hog!
 *	...
 *	Not anymore >:)
 *	
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

#define MILLION 1000000

void thread2(void *arg)
{
	(void)arg;

	printf("Not anymore >:)\n");
	exit(EXIT_SUCCESS);
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);

	while (1) {
		printf("I am a hog!\n");
		for (int i = 0; i < MILLION; ++i);
	}
}

int main(void)
{
	uthread_run(true, thread1, NULL);
	printf("Returned!\n");
	return 0;
}
