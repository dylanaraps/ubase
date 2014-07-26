/* See LICENSE file for copyright and license details. */
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "util.h"

static void
usage(void)
{
	eprintf("usage: %s [n|y]\n", argv0);
}

int
main(int argc, char *argv[])
{
	struct stat sb;
	mode_t mode;

	ARGBEGIN {
	default:
		usage();
	} ARGEND;

	if (argc > 1)
		usage();

	if (isatty(STDIN_FILENO) == 0)
		eprintf("stdin: not a tty\n");

	if (fstat(STDIN_FILENO, &sb) < 0)
		eprintf("fstat stdin:");

	if (argc == 0) {
		puts(sb.st_mode & (S_IWGRP | S_IWOTH) ? "is y" : "is n");
		return EXIT_SUCCESS;
	}

	if (argv[0][0] == 'y' && argv[0][1] == '\0')
		mode = sb.st_mode | S_IWGRP | S_IWOTH;
	else if (argv[0][0] == 'n' && argv[0][1] == '\0')
		mode = sb.st_mode & ~(S_IWGRP | S_IWOTH);
	else
		usage();

	if (fchmod(STDIN_FILENO, mode) < 0)
		eprintf("fchmod stdin:");

	return EXIT_SUCCESS;
}
