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
#include "horace/horace_error.h"
#include "horace/signal_set.h"
#include "horace/terminate_flag.h"
#include "horace/hostname.h"
#include "horace/string_attribute.h"
#include "horace/record.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/session_listener.h"
#include "horace/session_reader.h"
#include "horace/session_writer.h"
#include "horace/endpoint.h"
#include "horace/session_listener_endpoint.h"
#include "horace/session_writer_endpoint.h"

using namespace horace;

// Pointers to all forwarding threads.
std::vector<std::thread> threads;

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

/** Handle an unexpected record type.
 * @param rec the record to be handled
 */
void handle_unexpected_record(session_reader& src_sr, const record& rec) {
	if (rec.channel_id() == channel_error) {
		auto msgattr = rec.find_one<string_attribute>(attrid_message);
		throw horace_error("remote error: " + msgattr.content());
	} else if (rec.channel_id() == channel_warning) {
		auto msgattr = rec.find_one<string_attribute>(attrid_message);
		if (log->enabled(logger::log_warning)) {
			log_message msg(*log, logger::log_warning);
			msg << "remote warning: " << msgattr.content();
		}
		src_sr.write(rec);
	} else {
		throw horace_error("unexpected record type from destination");
	}
}

/** Forward records for a single source ID.
 * @param src_sr the source
 * @param dst_swep the destination
 */
void forward_one(session_reader& src_sr, session_writer_endpoint& dst_swep) {
	uint64_t expected_seqnum = 0;
	bool initial_seqnum = true;

	// Read the session record.
	std::unique_ptr<record> srec = src_sr.read();
	if (srec->channel_id() != channel_session) {
		throw horace_error("session record expected");
	}
	srec->log(*log);

	// Check the protocol version in the session record.
	if (protocol_version) {
		if (srec->contains(attrid_protocol)) {
			unsigned int srec_protocol_version =
				srec->find_one<unsigned_integer_attribute>(
					attrid_protocol).content();
			if (srec_protocol_version != *protocol_version) {
				throw horace_error("unsupported protocol version");
			}
		} else {
			throw horace_error("unspecified protocol version");
		}
	}

	// Create a session writer using the source ID from the
	// session record.
	std::string srcid = srec->find_one<string_attribute>(
		attrid_source).content();
	std::unique_ptr<session_writer> dst_sw = dst_swep.make_session_writer(srcid);

	// Attempt to write the session record.
	dst_sw->write(*srec);

	// Copy records from source to destination. Watch for
	// session records and sync records, which require special
	// handling.
	while (true) {
		// Ensure that termination is picked up, even if
		// the thread never blocks.
		terminating.poll();

		// Check whether a record is readable from the destination
		// endpoint (error or warning).
		if (dst_sw->readable()) {
			auto rec = dst_sw->read();
			rec->log(*log);
			handle_unexpected_record(src_sr, *rec);
		}

		// Read record from source endpoint.
		std::unique_ptr<record> rec = src_sr.read();

		// Log the record.
		rec->log(*log);

		// Attempt to write record to destination.
		dst_sw->write(*rec);

		// Perform any special handling required by specific
		// record types.
		switch (rec->channel_id()) {
		case channel_sync:
			// Sync records must be acknowledged.
			while (true) {
				auto rec = dst_sw->read();
				rec->log(*log);
				if (rec->channel_id() == channel_sync) {
					src_sr.write(*rec);
					break;
				} else {
					handle_unexpected_record(src_sr, *rec);
				}
			}
		default:
			if (rec->is_event()) {
				// Update sequence number, log any discontinuties.
				uint64_t seqnum = rec->find_one<unsigned_integer_attribute>(
					attrid_seqnum).content();
				if (initial_seqnum) {
					if (log->enabled(logger::log_notice)) {
						log_message msg(*log, logger::log_notice);
						msg << "forwarding from seqnum=" << seqnum;
					}
					initial_seqnum = false;
				} else if (seqnum != expected_seqnum) {
					if (log->enabled(logger::log_warning)) {
						log_message msg(*log, logger::log_warning);
						msg << "seqnum discontinuity (" <<
							"expected=" << expected_seqnum << ", " <<
							"observed=" << seqnum << ")";
					}
				}
				expected_seqnum = seqnum + 1;
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
		} catch (terminate_exception&) {
			return;
		} catch (std::exception& ex) {
			if (log->enabled(logger::log_err)) {
				log_message msg(*log, logger::log_err);
				msg << ex.what();
			}

			// Either retry if that is possible, or response with
			// an error record if not.
			retry = src_sr->reset();
			if (retry) {
				if (log->enabled(logger::log_notice)) {
					log_message msg(*log, logger::log_notice);
					msg << "will retry following error";
				}
			} else {
				try {
					attribute_list attrs;
					attrs.insert(std::make_unique<string_attribute>(
						attrid_message, ex.what()));
					auto errrec = std::make_unique<record>(
						channel_error, std::move(attrs));
					src_sr->write(*errrec);
				} catch (terminate_exception&) {
					return;
				} catch (std::exception& ex) {
					if (log->enabled(logger::log_err)) {
						log_message msg(*log, logger::log_err);
						msg << ex.what();
					}
				}
			}
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
			std::thread th(
				forward_one_with_retry,
				std::move(src_sr), std::ref(dst_swep));
			threads.push_back(std::move(th));
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
	std::thread all_th(forward_all,
		std::ref(*src_slep), std::ref(*dst_swep));
	threads.push_back(std::move(all_th));

	// Wait for terminating signal to be raised.
	int raised = masked_signals.wait();

	// Stop forwarding and exit.
	std::cerr << strsignal(raised) << std::endl;
	terminating = true;
	for (auto& th : threads) {
		th.join();
	}
}
