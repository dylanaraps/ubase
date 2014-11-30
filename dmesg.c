/* See LICENSE file for copyright and license details. */
#include <sys/klog.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

static void dmesg_show(int fd, const void *buf, size_t n);

enum {
	SYSLOG_ACTION_READ_ALL = 3,
	SYSLOG_ACTION_CLEAR = 5,
	SYSLOG_ACTION_CONSOLE_LEVEL = 8,
	SYSLOG_ACTION_SIZE_BUFFER = 10
};

static void
usage(void)
{
	eprintf("usage: [-Ccr] [-n level] %s\n", argv0);
}

int
main(int argc, char *argv[])
{
	int n;
	char *buf;
	int cflag = 0;
	long level;

	ARGBEGIN {
	case 'C':
		if (klogctl(SYSLOG_ACTION_CLEAR, NULL, 0) < 0)
			eprintf("klogctl:");
		return 0;
	case 'c':
		cflag = 1;
		break;
	case 'r':
		break;
	case 'n':
		level = estrtol(EARGF(usage()), 10);
		if (klogctl(SYSLOG_ACTION_CONSOLE_LEVEL, NULL, level) < 0)
			eprintf("klogctl:");
		return 0;
	default:
		usage();
	} ARGEND;

	n = klogctl(SYSLOG_ACTION_SIZE_BUFFER, NULL, 0);
	if (n < 0)
		eprintf("klogctl:");

	buf = emalloc(n);

	n = klogctl(SYSLOG_ACTION_READ_ALL, buf, n);
	if (n < 0)
		eprintf("klogctl:");

	dmesg_show(1, buf, n);

	if (cflag && klogctl(SYSLOG_ACTION_CLEAR, NULL, 0) < 0)
		eprintf("klogctl:");

	free(buf);
	return 0;
}

static void
dmesg_show(int fd, const void *buf, size_t n)
{
	const char *p = buf;
	ssize_t r;

	r = write(fd, p, n);
	if (r < 0)
		eprintf("write:");
	if (r > 0 && p[r - 1] != '\n')
		putchar('\n');
}
