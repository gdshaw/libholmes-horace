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
#include "horace/unsigned_integer_attribute.h"
#include "horace/binary_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"
#include "horace/session_builder.h"
#include "horace/event_reader.h"
#include "horace/session_writer.h"
#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"
#include "horace/session_writer_endpoint.h"

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
	out << "  -v  increase verbosity of log messages" << std::endl;
}

void capture(session_builder& session, uint64_t& seqnum,
	event_reader& src_er, session_writer_endpoint& dst_swep,
	hash* hashfn, keypair* kp) {

	std::unique_ptr<session_writer> dst_sw =
		dst_swep.make_session_writer(session.source_id());

	std::unique_ptr<record> srec = session.build();
	std::unique_ptr<binary_attribute> hattr;
	try {
		try {
			dst_sw->write(*srec);
		} catch (terminate_exception&) {
			throw;
		} catch (std::exception& ex) {
			throw retry_exception();
		}
		srec->log(*log);

		while (true) {
			// Ensure that termination is picked up, even if
			// the thread never blocks.
			terminating.poll();

			// Read record from source.
			const record& rec = src_er.read();

			// Append sequence number to record.
			// This is the same method as was previously used in
			// spoolfile_writer, and is known to be inefficient.
			// It was tolerable in that context because it was
			// invoked only once per spoolfile. Here it is
			// applied to every event record, and effectively
			// undoes efforts made elsewhere to avoid copying.
			// A more efficient solution would be desirable, but
			// the current event reader API does not allow for one.
			attribute_list attrs = rec.attributes();
			unsigned_integer_attribute seqnum_attr(attrid_seqnum, seqnum++);
			attrs.append(seqnum_attr);
			if (hattr) {
				attrs.append(std::move(hattr));
			}
			record nrec(rec.channel_number(), std::move(attrs));

			// Attempt to write record to destination.
			try {
				dst_sw->write(nrec);
			} catch (terminate_exception&) {
				throw;
			} catch (std::exception& ex) {
				throw retry_exception();
			}

			if (hashfn) {
				nrec.write(*hashfn);
				const void* hash = hashfn->final();
				size_t hash_len = hashfn->length();

				hattr = std::make_unique<binary_attribute>(attrid_hash,
					hash_len, hash);

				if (kp) {
					std::string sig = kp->sign(hash, hash_len);
					attribute_list sigattrs;
					sigattrs.append(std::make_unique<binary_attribute>(
						attrid_sig, sig.length(), sig.data()));
					record sigrec(channel_signature, std::move(sigattrs));
					try {
						dst_sw->write(sigrec);
					} catch (terminate_exception&) {
						throw;
					} catch (std::exception& ex) {
						throw retry_exception();
					}
				}
			}
		}
	} catch (retry_exception&) {
		throw;
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	// Write end of session timestamp to session channel.
	attribute_list attrs;
	attrs.append(srec->find_one<string_attribute>(attrid_source).clone());
	attrs.append(srec->find_one<timestamp_attribute>(attrid_ts_begin).clone());
	attrs.append(std::make_unique<timestamp_attribute>(attrid_ts_end));
	std::unique_ptr<record> erec = std::make_unique<record>(channel_session, attrs);
	dst_sw->write(*erec);
	erec->log(*log);
}

void capture_with_retry(session_builder& session, event_reader& src_er,
	session_writer_endpoint& dst_swep, hash* hashfn, keypair* kp) {

	uint64_t seqnum = 0;
	bool retry = true;
	while (retry) {
		retry = false;
		try {
			capture(session, seqnum, src_er, dst_swep, hashfn, kp);
		} catch (retry_exception&) {
			retry = true;
			if (log->enabled(logger::log_err)) {
				log_message msg(*log, logger::log_err);
				msg << "error during capture (will retry)";
			}
		} catch (std::exception& ex) {
			std::cerr << ex.what() << std::endl;
		}
	}
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
	address_filter addrfilt;
	int severity = logger::log_warning;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+D:hk:S:vx:")) != -1) {
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

	// Make event reader for source endpoint.
	event_reader_endpoint* src_erep =
		dynamic_cast<event_reader_endpoint*>(src_ep.get());
	if (!src_erep) {
		std::cerr << "Source endpoint is unable to capture events."
			<< std::endl;
		exit(1);
	}
	session_builder session(source_id);
	std::unique_ptr<event_reader> src_er =
		src_erep->make_event_reader(session);

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

	// Attach address filter to event reader, but only if it is non-empty.
	if (!addrfilt.empty()) {
		src_er->attach(addrfilt);
	}

	// Capture events.
	std::thread capture_thread(capture_with_retry, std::ref(session),
		std::ref(*src_er), std::ref(*dst_swep), hashfn.get(), kp.get());

	// Wait for terminating signal to be raised.
	int raised = terminating_signals.wait();

	// Stop listening and exit.
	std::cerr << strsignal(raised) << std::endl;
	terminating.set();
	pthread_kill(capture_thread.native_handle(), SIGALRM);
	capture_thread.join();
}

int main(int argc, char* argv[]) {
	try {
		main2(argc, argv);
	} catch (const std::exception& ex) {
		std::cerr << ex.what() << std::endl;
		exit(1);
	}
}
