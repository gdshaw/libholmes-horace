// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cerrno>
#include <climits>
#include <cstring>
#include <iostream>

#include <unistd.h>
#include <getopt.h>

#include "horace/libc_error.h"

/** Print help text.
 * @param out the ostream to which the help text should be written
 */
void write_help(std::ostream& out) {
	out << "Usage: horace [<options>] command [<args>]" << std::endl;
	out << std::endl;
	out << "Options:" << std::endl;
	out << std::endl;
	out << "  -h  display this help text then exit" << std::endl;
	out << "  -V  display version number then exit" << std::endl;
}

/** Print version information.
 * @param out the ostream to which the version information should be written.
 */
void write_version(std::ostream& out) {
	out << "LibHolmes-HORACE (experimental version)" << std::endl;
}

extern char** environ;

int main(int argc, char* argv[]) {
	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+hV")) != -1) {
		switch (opt) {
		case 'h':
			write_help(std::cout);
			return 0;
		case 'V':
			write_version(std::cout);
			return 0;
		}
	}

	// Error if no command given.
	if (optind == argc) {
		std::cerr << "Command not specified. See horace -h."
			<< std::endl;
		exit(1);
	}

	// Construct pathname for executable corresponding to command.
	// Also need the filename, for use as what will become argv[0].
	// This is done by recording the index into the pathname at
	// which the filename begins.
	const char* cmdprefix = "horace-";
	const char* cmdname = argv[optind];
	char pathname[PATH_MAX];
	int filename_idx = 0;
	if (snprintf(pathname, sizeof(pathname), "%s/%s/bin/%n%s%s",
		LIBEXECDIR, PKGNAME, &filename_idx,
		cmdprefix, cmdname) >= sizeof(pathname)) {

		std::cerr << "Command too long." << std::endl;
		exit(1);
	}

	// Invoke the executable at the constructed pathname.
	argv[optind] = pathname + filename_idx;
	if (execve(pathname, argv + optind, environ) == -1) {
		// Want to give user-friendly error message for the case
		// where there is no executable corresponding to the given
		// command name. This would result in errno being set to
		// ENOENT, but is not the only way that could happen.
		// Therefore, if execve fails for any reason, try testing
		// whether the executable exists before deciding on how
		// to report the error.
		int execve_errno = errno;
		if ((access(pathname, F_OK) == -1) && (errno == ENOENT)) {
			std::cerr << "Command not recognised. See horace -h."
				<< std::endl;
			exit(1);
		} else {
			throw horace::libc_error(execve_errno);
		}
	}
}
