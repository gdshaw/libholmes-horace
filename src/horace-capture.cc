// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <memory>
#include <iostream>

#include <getopt.h>

#include "horace/endpoint.h"

/** Print help text.
 * @param out the ostream to which the help text should be written
 */
void write_help(std::ostream& out) {
	out << "Usage: horace capture [<args>] <source> <destination>" << std::endl;
	out << std::endl;
	out << "Options:" << std::endl;
	out << std::endl;
	out << "  -h  display this help text then exit" << std::endl;
}

int main(int argc, char* argv[]) {
	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+hV")) != -1) {
		switch (opt) {
		case 'h':
			write_help(std::cout);
			return 0;
		}
	}

	if (optind == argc) {
		std::cerr << "Source endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<horace::endpoint> src_ep =
		horace::endpoint::make(argv[optind++]);

	if (optind == argc) {
		std::cerr << "Destination endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<horace::endpoint> dst_ep =
		horace::endpoint::make(argv[optind++]);

	if (optind != argc) {
		std::cerr << "Too many arguments on command line."
			<< std::endl;
	}
}
