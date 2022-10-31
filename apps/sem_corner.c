/*
 * Semaphore corner case test
 *
 * Should print the following, with a 3 second delay where indicated:
 * 
 *      thread2
 *      thread3
 *	<wait 3 seconds>
 *	Kirby Super Star Ultra
 * 
 * then exit. If the output is
 * 
 *      thread2
 *      thread3
 *      thread1
 * 
 * followed by immediate termination, then corner case protection did not work. 
 */

#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include <sem.h>
#include <uthread.h>

#define UNUSED(x) (void)(x)

int total;
sem_t sem;

void test_handler(int signum)
{
	UNUSED(signum);

	printf("Kirby Super Star Ultra\n");
	exit(EXIT_SUCCESS);
}

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
	struct sigaction sa;
	sem = sem_create(0);
	total = 0;

	/* Set up handler for alarm */
	sa.sa_handler = test_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);

	/* Configure alarm */
	alarm(3);

	uthread_run(false, thread1, NULL);

	if(sem_destroy(sem) != 0) printf("FAILURE\n");

	return 0;
}
