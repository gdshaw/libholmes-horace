// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cctype>
#include <cstring>
#include <memory>
#include <iostream>
#include <thread>

#include <getopt.h>

#include "horace/retry_exception.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/stderr_logger.h"
#include "horace/signal_set.h"
#include "horace/empty_signal_handler.h"
#include "horace/terminate_flag.h"
#include "horace/hostname.h"
#include "horace/inet4_netblock.h"
#include "horace/inet6_netblock.h"
#include "horace/address_filter.h"
#include "horace/hash.h"
#include "horace/keypair.h"
#include "horace/record.h"
#include "horace/session_builder.h"
#include "horace/event_reader.h"
#include "horace/new_session_writer.h"
#include "horace/event_source.h"
#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"

using namespace horace;

/** Print help text.
 * @param out the ostream to which the help text should be written
 */
void write_help(std::ostream& out) {
	out << "Usage: horace capture [<args>] <source> <destination>" << std::endl;
	out << std::endl;
	out << "Options:" << std::endl;
	out << std::endl;
	out << "  -h  display this help text then exit" << std::endl;
	out << "  -S  set source identifier" << std::endl;
	out << "  -x  exclude address or netblock" << std::endl;
	out << "  -D  hash messages with given digest function" << std::endl;
	out << "  -k  sign messages using key in given file" << std::endl;
	out << "  -R  set minimum time in milliseconds between signed events" << std::endl;
	out << "  -v  increase verbosity of log messages" << std::endl;
}

int main2(int argc, char* argv[]) {
	// Mask signals.
	masked_signals.mask();

	// Install empty signal handlers.
	empty_signal_handler sigint_handler(SIGINT);
	empty_signal_handler sigquit_handler(SIGQUIT);
	empty_signal_handler sigterm_handler(SIGTERM);
	empty_signal_handler sigalrm_handler(SIGALRM);

	// Get hostname for use as source ID.
	std::string source_id = hostname();

	// Initialise default options.
	const char* hashfn_name = 0;
	const char* keyfile_pathname = 0;
	unsigned long sigrate = 0;
	address_filter addrfilt;
	int severity = logger::log_warning;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+D:hk:R:S:vx:")) != -1) {
		switch (opt) {
		case 'D':
			hashfn_name = optarg;
			break;
		case 'h':
			write_help(std::cout);
			return 0;
		case 'k':
			keyfile_pathname = optarg;
			break;
		case 'R':
			sigrate = std::stol(optarg);
			break;
		case 'S':
			source_id = std::string(optarg);
			break;
		case 'v':
			if (severity < logger::log_debug) {
				severity += 1;
			}
			break;
		case 'x':
			if (strchr(optarg, ':') != 0) {
				inet6_netblock nb(optarg);
				addrfilt.add(nb);
			} else {
				inet4_netblock nb(optarg);
				addrfilt.add(nb);
			}
			break;
		}
	}

	// Initialise logger.
	log = std::make_unique<stderr_logger>();
	log->severity(severity);

	// Select hash function.
	std::unique_ptr<hash> hashfn;
	if (hashfn_name) {
		hashfn = hash::make(hashfn_name);
	}

	// Load keyfile.
	std::unique_ptr<keypair> kp;
	if (keyfile_pathname) {
		kp = keypair::load(keyfile_pathname);
	}

	// Validate source ID.
	for (char c : source_id) {
		if (!isalnum(c) && (c != '-') && (c != '.')) {
			std::cerr << "Invalid source ID" << std::endl;
			exit(1);
		}
	}

	// Parse source endpoint.
	if (optind == argc) {
		std::cerr << "Source endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<endpoint> src_ep =
		endpoint::make(argv[optind++]);

	// Parse destination endpoint.
	if (optind == argc) {
		std::cerr << "Destination endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<endpoint> dst_ep =
		endpoint::make(argv[optind++]);

	if (optind != argc) {
		std::cerr << "Too many arguments on command line."
			<< std::endl;
	}

	// Make a new_session_writer for destination endpoint.
	new_session_writer dst(*dst_ep, source_id, hashfn.get(),
		kp.get(), sigrate);

	// Make an event_source for source endpoint.
	session_builder sb(source_id);
	event_source src(*src_ep, dst, sb);

	// Attach address filter to event reader, but only if it is non-empty.
	if (!addrfilt.empty()) {
		src.attach(addrfilt);
	}

	// Start the session.
	std::unique_ptr<record> srec = sb.build();
	dst.begin_session(*srec);

	// Start capturing events.
	src.start();

	// Wait for terminating signal to be raised.
	int raised = terminating_signals.wait();

	// Stop listening and exit.
	std::cerr << strsignal(raised) << std::endl;
	terminating.set();
	src.stop();
	dst.end_session();
}

int main(int argc, char* argv[]) {
	try {
		main2(argc, argv);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		exit(1);
	}
}
