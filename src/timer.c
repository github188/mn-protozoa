/*
 * protozoa -- CCTV transcoder / mixer for PTZ
 * Copyright (C) 2008  Minnesota Department of Transportation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <fcntl.h>	/* for fcntl, F_SETFL, O_NONBLOCK */
#include <signal.h>	/* for sigfillset, sigaction, SA_RESTART, SIGALRM */
#include <stdio.h>	/* for NULL */
#include <sys/time.h>	/* for setitimer, ITIMER_REAL, struct itimerval */
#include <unistd.h>	/* for write */
#include "timer.h"

struct timer timer_singleton;

static void timer_handler(int signo) {
	static const char c = 0;

	write(timer_singleton.pipe_fd[PIPE_WRITE], &c, 1);
}

static int timer_install_handler() {
	struct sigaction sa;

	sa.sa_handler = &timer_handler;
	sa.sa_flags = SA_RESTART;
	if(sigfillset(&sa.sa_mask) < 0)
		return -1;
	return sigaction(SIGALRM, &sa, &timer_singleton.sa_original);
}

struct timer *timer_init() {
	if(pipe(timer_singleton.pipe_fd) < 0)
		return NULL;
	if(fcntl(timer_singleton.pipe_fd[PIPE_READ], F_SETFL, O_NONBLOCK) < 0)
		goto fail;
	if(fcntl(timer_singleton.pipe_fd[PIPE_WRITE], F_SETFL, O_NONBLOCK) < 0)
		goto fail;
	if(timer_install_handler() < 0)
		goto fail;
	return &timer_singleton;
fail:
	close(timer_singleton.pipe_fd[PIPE_WRITE]);
	close(timer_singleton.pipe_fd[PIPE_READ]);
	return NULL;
}

static void timer_remove_handler() {
	sigaction(SIGALRM, &timer_singleton.sa_original, NULL);
}

void timer_destroy() {
	timer_remove_handler();
	close(timer_singleton.pipe_fd[PIPE_WRITE]);
	close(timer_singleton.pipe_fd[PIPE_READ]);
}

int timer_arm(unsigned int msec) {
	timer_singleton.itimer.it_interval.tv_sec = 0;
	timer_singleton.itimer.it_interval.tv_usec = 0;
	timer_singleton.itimer.it_value.tv_sec = msec / 1000;
	timer_singleton.itimer.it_value.tv_usec = msec % 1000 * 1000;

	return setitimer(ITIMER_REAL, &timer_singleton.itimer, NULL);
}

int timer_disarm() {
	return timer_arm(0);
}

int timer_get_fd() {
	return timer_singleton.pipe_fd[PIPE_READ];
}