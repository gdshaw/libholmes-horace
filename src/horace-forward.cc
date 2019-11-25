// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cstring>
#include <vector>
#include <iostream>
#include <thread>

#include <getopt.h>

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/stderr_logger.h"
#include "horace/retry_exception.h"
#include "horace/horace_error.h"
#include "horace/signal_set.h"
#include "horace/empty_signal_handler.h"
#include "horace/terminate_flag.h"
#include "horace/hostname.h"
#include "horace/string_attribute.h"
#include "horace/record.h"
#include "horace/session_listener.h"
#include "horace/session_reader.h"
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
	out << "  -v  increase verbosity of log messages" << std::endl;
}

/** Forward records for a single source ID.
 * @param src_sr the source
 * @param dst_swep the destination
 */
void forward_one(session_reader& src_sr, session_writer_endpoint& dst_swep) {
	uint64_t current_seqnum = 0;
	uint64_t expected_seqnum = 0;
	bool initial_seqnum = true;

	// Read the start of session record. Keep a copy of it,
	// unless/until it is superseded by another start of
	// session record.
	std::unique_ptr<record> srec = src_sr.read();
	if (srec->channel_number() != channel_session) {
		throw horace_error("start of session record expected");
	}
	srec->log(*log);

	// Create a session writer using the source ID from the
	// start of session record.
	std::string source_id = srec->find_one<string_attribute>(
		attr_source).content();
	std::unique_ptr<session_writer> dst_sw = dst_swep.make_session_writer(source_id);

	// Attempt to write the start of session record.
	try {
		dst_sw->write(*srec);
	} catch (terminate_exception&) {
		throw;
	} catch (std::exception& ex) {
		throw retry_exception(ex);
	}

	// Copy records from source to destination. Watch for
	// start of session records and sync records, which
	// require special handling.
	while (true) {
		// Ensure that termination is picked up, even if
		// the thread never blocks.
		terminating.poll();

		// Read record from source endpoint.
		std::unique_ptr<record> rec = src_sr.read();

		// Log the record.
		rec->log(*log);

		// Update sequence number, log any discontinuties.
		current_seqnum = rec->update_seqnum(current_seqnum);
		if (initial_seqnum) {
			if (log->enabled(logger::log_notice)) {
				log_message msg(*log, logger::log_notice);
				msg << "forwarding from seqnum=" << current_seqnum;
			}
			initial_seqnum = false;
		} else if (current_seqnum != expected_seqnum) {
			if (log->enabled(logger::log_warning)) {
				log_message msg(*log, logger::log_warning);
				msg << "seqnum discontinuity (" <<
					"expected=" << expected_seqnum << ", " <<
					"observed=" << current_seqnum << ")";
			}
		}

		// Attempt to write record to destination.
		try {
			dst_sw->write(*rec);
		} catch (terminate_exception&) {
			throw;
		} catch (std::exception& ex) {
			throw retry_exception(ex);
		}

		// Perform any special handling required by specific
		// record types.
		switch (rec->channel_number()) {
		case channel_session:
			// Keep a copy of the most recent start of
			// session record.
			// Note: must not make any further reference to rec
			// following this statement.
			srec = std::move(rec);
			current_seqnum = 0;
			break;
		case channel_sync:
			// Sync records must be acknowledged.
			{
				std::unique_ptr<record> arec;
				try {
					arec = dst_sw->read();
				} catch (terminate_exception&) {
					throw;
				} catch (std::exception& ex) {
					throw retry_exception(ex);
				}
				src_sr.write(*arec);
				arec->log(*log);
				break;
			}
		default:
			if (rec->is_event()) {
				current_seqnum += 1;
				expected_seqnum = current_seqnum;
			}
		}
	}
}

/** Forward records for a single source ID, with retry on error.
 * @param src_sr the source
 * @param dst_swep the destination
 */
void forward_one_with_retry(std::unique_ptr<session_reader> src_sr,
	session_writer_endpoint& dst_swep) {

	bool retry = true;
	while (retry) {
		retry = false;
		try {
			forward_one(*src_sr, dst_swep);
		} catch (retry_exception&) {
			retry = src_sr->reset();

			if (log->enabled(logger::log_err)) {
				log_message msg(*log, logger::log_err);
				msg << "error during forwarding";
				if (retry) {
					msg << " (will retry)";
				}
			}
		} catch (terminate_exception&) {
			// No action.
		} catch (std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
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
			// Ensure that termination is picked up, even if
			// the thread never blocks (which is very unlikely
			// to be a problem in this instance, but there is
			// little harm in making sure).
			terminating.poll();

			std::unique_ptr<session_reader> src_sr =
				src_sl->accept();
			std::thread* th = new std::thread(
				forward_one_with_retry,
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

	// Initialise default options.
	int severity = logger::log_warning;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+hv")) != -1) {
		switch (opt) {
		case 'h':
			write_help(std::cout);
			return 0;
		case 'v':
			if (severity < logger::log_debug) {
				severity += 1;
			}
			break;
		}
	}

	// Initialise logger.
	log = std::make_unique<stderr_logger>();
	log->severity(severity);

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
