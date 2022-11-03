#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

#define UNUSED(x) (void)(x)

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct itimerval it_val, it_val_old;
struct sigaction sa_new, sa_old;
sigset_t ss;

long count = 0;

void alarm_handler(int signum)
{
	UNUSED(signum);

	// Force yield
	uthread_yield();
}

void preempt_disable(void)
{
	// Block signals
	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void)
{
	// Unblock signals
	sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_start(bool preempt)
{
	// Initialize set s.t. preempt_{disable, enable} doesn't crash process
	sigemptyset(&ss);

	// Don't set up if not using
	if (!preempt)
		return;

	// Prepare signals to block
	sigaddset(&ss, SIGVTALRM);
	sigaddset(&ss, SIGALRM);

	// Handle using sigaction()
	sa_new.sa_handler = &alarm_handler;
	sigemptyset(&sa_new.sa_mask);
	sa_new.sa_flags = 0;
	sigaction(SIGVTALRM, &sa_new, &sa_old); // save old configuration

	// Create alarm that pops off every 100ms
	it_val.it_value.tv_sec = 0;
	it_val.it_value.tv_usec = 1000000 / HZ;
	it_val.it_interval.tv_sec = 0;
	it_val.it_interval.tv_usec = 1000000 / HZ;

	if (setitimer(ITIMER_VIRTUAL, &it_val, &it_val_old) == -1) { // save old configuration
		perror("setitimer");
		exit(1);
	}
}

void preempt_stop(void)
{
	preempt_disable();

	// Restore previous signal handler
	sigaction(SIGALRM, &sa_old, NULL);

	// Restore previous timer config
	if (setitimer(ITIMER_REAL, &it_val_old, NULL) == -1) {
		perror("setitimer");
		exit(1);
	}
}

