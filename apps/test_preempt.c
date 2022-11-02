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
 * After a certain number of seconds (~5-10)
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

#define MOD 1000000

bool flag = true;

void thread2(void* arg)
{
	(void)arg;

	printf("Not anymore >:)\n");
	flag = false;
}

void thread1(void* arg)
{
	(void)arg;

	uthread_create(thread2, NULL);

	//Run until flag is false
	for (int i = 0; flag; ++i) {
		if (i % MOD != 0) continue;

		printf("I am a hog!\n");
		i = 0;
	}
}

int main(void)
{
	uthread_run(true, thread1, NULL);

	return 0;
}
