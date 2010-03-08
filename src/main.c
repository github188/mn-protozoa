/*
 * protozoa -- CCTV transcoder / mixer for PTZ
 * Copyright (C) 2006-2010  Minnesota Department of Transportation
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
#include <string.h>	/* for strerror */
#include <unistd.h>	/* for daemon */
#include <sys/errno.h>	/* for errno */

#include "config.h"
#include "poller.h"

#define VERSION "0.37"
#define BANNER "protozoa: v" VERSION "  Copyright (C) 2006-2010  Mn/DOT"

static const char *CONF_FILE = "/etc/protozoa.conf";
static const char *LOG_FILE = "/var/log/protozoa";

int main(int argc, char* argv[]) {
	int i;
	int rc;
	struct poller poll;
	struct packet_counter *counter;
	struct log log;
	struct config cfg;
	bool daemonize = false;
	bool dryrun = false;

	rc = 0;
	log_init(&log);
	for(i = 0; i < argc; i++) {
		if(strcmp(argv[i], "--daemonize") == 0)
			daemonize = true;
		if(strcmp(argv[i], "--debug") == 0)
			log.debug = true;
		if(strcmp(argv[i], "--dryrun") == 0)
			dryrun = true;
		if(strcmp(argv[i], "--packet") == 0)
			log.packet = true;
		if(strcmp(argv[i], "--stats") == 0)
			log.stats = true;
	}
	if(daemonize) {
		if(log_open_file(&log, LOG_FILE) == NULL) {
			log_println(&log, "Cannot open: %s", LOG_FILE);
			rc = (errno ? errno : -1);
			goto out;
		}
	}
	log_println(&log, BANNER);
	if(log.stats)
		counter = packet_counter_new(&log);
	else
		counter = NULL;
	if(config_init(&cfg, &log, counter) == NULL) {
		rc = (errno ? errno : -1);
		goto out;
	}
	if(config_read(&cfg, CONF_FILE) <= 0) {
		log_println(&log, "Check configuration file: %s", CONF_FILE);
		rc = (errno ? errno : -1);
		goto out_0;
	}
	if(dryrun)
		goto out_0;
	if(poller_init(&poll, cfg.n_channels, config_cede_channels(&cfg),
		cfg.defer) == NULL)
	{
		rc = (errno ? errno : -1);
		goto out_0;
	}
	if(daemonize) {
		if(daemon(0, 0) < 0) {
			rc = (errno ? errno : -1);
			goto out_0;
		}
	}
	rc = poller_loop(&poll);
out_0:
	config_destroy(&cfg);
	free(counter);
out:
	if(rc > 0)
		log_println(&log, "Error: %s", strerror(rc));
	log_destroy(&log);
	return rc;
}
