#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct itimerval it_val;
struct sigaction sa_new, sa_old;

void alarm_handler(int signum)
{
	(void)signum;
	// do stuff
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}

void preempt_start(bool preempt)
{
	// Don't set up if not using
	if (!preempt)
		return;

	//handle using sigaction()
	sa_new.sa_handler = &alarm_handler;
	sigemptyset(&sa_new.sa_mask);
	sa_new.sa_flags = 0;
	sigaction(SIGALRM, &sa_new, &sa_old);

	// Create alarm that pops off every 100ms
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 1000000 / HZ;
	it_val.it_interval.tv_sec = 0;
	it_val.it_interval.tv_usec = 1000000 / HZ;

	if (setitimer(ITIMER_VIRTUAL, &it_val, NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
}

void preempt_stop(void)
{
	// Reset signal handling to default
	sigaction(SIGALRM, &sa_old, NULL);

	// Set timer s.t. it stops running
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 0;

	if (setitimer(ITIMER_VIRTUAL, &it_val, NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
}

