// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cstring>
#include <vector>
#include <iostream>
#include <thread>

#include <getopt.h>

#include "horace/horace_error.h"
#include "horace/signal_set.h"
#include "horace/empty_signal_handler.h"
#include "horace/terminate_flag.h"
#include "horace/hostname.h"
#include "horace/record.h"
#include "horace/session_start_record.h"
#include "horace/session_writer.h"
#include "horace/endpoint.h"
#include "horace/session_listener_endpoint.h"
#include "horace/session_writer_endpoint.h"

using namespace horace;

// Pointers to all forwarding threads.
std::vector<std::thread*> threads;

/** Print help text.
 * @param out the ostream to which the help text should be written
 */
void write_help(std::ostream& out) {
	out << "Usage: horace forward [<args>] <source> <destination>" << std::endl;
	out << std::endl;
	out << "Options:" << std::endl;
	out << std::endl;
	out << "  -h  display this help text then exit" << std::endl;
}

/** Forward records for a single source ID.
 * @param src_sr the source
 * @param dst_swep the destination
 */
void forward_one(std::unique_ptr<session_reader> src_sr, session_writer_endpoint& dst_swep) {
	try {
		// Read the start of session record. Keep a copy of it,
		// unless/until it is superseded by another start of
		// session record.
		std::unique_ptr<record> srec = src_sr->read();
		if (srec->type() != record::REC_SESSION_START) {
			throw horace_error("start of session record expected");
		}

		// Create a session writer using the source ID from the
		// start of session record.
		std::string source_id = dynamic_cast<session_start_record&>(*srec.get())
			.source_attr().source_id();
		std::unique_ptr<session_writer> dst_sw = dst_swep.make_session_writer(source_id);
		dst_sw->write(*srec);

		// Copy records from source to destination. Watch for
		// start of session records, keeping a copy of the most
		// recent one.
		while (true) {
			std::unique_ptr<record> rec = src_sr->read();
			dst_sw->write(*rec);
			if (rec->type() == record::REC_SESSION_START) {
				srec = std::move(rec);
			}
		}
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

/** Forward records for multiple source IDs.
 * @param src_slep the source
 * @param dst_swep the destination
 */
void forward_all(session_listener_endpoint& src_slep,
	session_writer_endpoint& dst_swep) {

	// Create a listener for the endpoint.
	std::unique_ptr<session_listener> src_sl =
		src_slep.make_session_listener();

	try {
		// Repeatedly accept connections from the listener,
		// creating a new forwarding thread for each one.
		while (true) {
			std::unique_ptr<session_reader> src_sr =
				src_sl->accept();
			std::thread* th = new std::thread(forward_one,
				std::move(src_sr), std::ref(dst_swep));
			threads.push_back(th);
		}
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

int main(int argc, char* argv[]) {
	// Mask signals.
	masked_signals.mask();

	// Install empty signal handlers.
	empty_signal_handler sigint_handler(SIGINT);
	empty_signal_handler sigquit_handler(SIGQUIT);
	empty_signal_handler sigterm_handler(SIGTERM);
	empty_signal_handler sigalrm_handler(SIGALRM);

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+h")) != -1) {
		switch (opt) {
		case 'h':
			write_help(std::cout);
			return 0;
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

	// Make session listener for source endpoint.
	session_listener_endpoint* src_slep =
		dynamic_cast<session_listener_endpoint*>(src_ep.get());
	if (!src_slep) {
		std::cerr << "Source endpoint is unable to listen for sessions."
			<< std::endl;
		exit(1);
	}

	// Parse destination endpoint.
	if (optind == argc) {
		std::cerr << "Destination endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<endpoint> dst_ep =
		endpoint::make(argv[optind++]);

	// Make session writer for destination endpoint.
	session_writer_endpoint* dst_swep =
		dynamic_cast<session_writer_endpoint*>(dst_ep.get());
	if (!dst_swep) {
		std::cerr << "Destination endpoint is unable to receive sessions."
			<< std::endl;
		exit(1);
	}

	if (optind != argc) {
		std::cerr << "Too many arguments on command line."
			<< std::endl;
	}

	// Forward events.
	std::thread* all_th = new std::thread(forward_all,
		std::ref(*src_slep), std::ref(*dst_swep));
	threads.push_back(all_th);

	// Wait for terminating signal to be raised.
	int raised = terminating_signals.wait();

	// Stop forwarding and exit.
	std::cerr << strsignal(raised) << std::endl;
	terminating.set();
	for (std::thread* th : threads) {
		pthread_kill(th->native_handle(), SIGALRM);
	}
	for (std::thread* th : threads) {
		th->join();
	}
}
