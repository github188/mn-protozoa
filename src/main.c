#include <stdio.h>	/* for printf */
#include <string.h>	/* for strerror */
#include <sys/errno.h>	/* for errno */

#include "sport.h"
#include "config.h"
#include "poller.h"

static const char *CONF_FILE = "protozoa.conf";

extern int errno;

int main(int argc, char* argv[])
{
	int n_ports;
	struct sport *port;
	struct poller poll;

	n_ports = config_read(CONF_FILE, &port);
	if(n_ports <= 0)
		goto fail;

	if(poller_init(&poll, n_ports, port) == NULL)
		goto fail;

	poller_loop(&poll);
fail:
	if(errno)
		printf("Error: %s\n", strerror(errno));
	return errno;
}