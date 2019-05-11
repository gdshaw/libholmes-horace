// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cstring>
#include <memory>
#include <iostream>
#include <thread>

#include <getopt.h>

#include "horace/signal_set.h"
#include "horace/empty_signal_handler.h"
#include "horace/terminate_flag.h"
#include "horace/hostname.h"
#include "horace/address_filter.h"
#include "horace/source_attribute.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/record.h"
#include "horace/record_builder.h"
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
	out << "  -x  exclude address or netblock" << std::endl;
}

void capture(const std::string& source_id, event_reader& src_er, session_writer& dst_sw) {
	try {
		record_builder srecb(record::REC_SESSION_START);
		srecb.append(std::make_shared<source_attribute>(source_id));
		srecb.append(std::make_shared<posix_timespec_attribute>());
		std::unique_ptr<record> srec = srecb.build();
		dst_sw.write(*srec);

		while (true) {
			const record& rec = src_er.read();
			dst_sw.write(rec);
		}
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}

	record_builder erecb(record::REC_SESSION_END);
	erecb.append(std::make_shared<posix_timespec_attribute>());
	std::unique_ptr<record> erec = erecb.build();
	dst_sw.write(*erec);
}

int main(int argc, char* argv[]) {
	// Mask signals.
	masked_signals.mask();

	// Install empty signal handlers.
	empty_signal_handler sigint_handler(SIGINT);
	empty_signal_handler sigquit_handler(SIGQUIT);
	empty_signal_handler sigterm_handler(SIGTERM);
	empty_signal_handler sigalrm_handler(SIGALRM);

	// Get hostname for use as source ID.
	hostname source_id;

	// Initialise using default options.
	address_filter addrfilt;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+hx:")) != -1) {
		switch (opt) {
		case 'h':
			write_help(std::cout);
			return 0;
		case 'x':
			inet4_netblock nb(optarg);
			addrfilt.add(nb);
			break;
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
	std::unique_ptr<event_reader> src_er =
		src_erep->make_event_reader();

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
	std::unique_ptr<session_writer> dst_sw =
		dst_swep->make_session_writer(source_id);

	if (optind != argc) {
		std::cerr << "Too many arguments on command line."
			<< std::endl;
	}

	// Attach address filter to event reader, but only if it is non-empty.
	if (!addrfilt.empty()) {
		src_er->attach(addrfilt);
	}

	// Capture events.
	std::thread capture_thread(capture, source_id, std::ref(*src_er), std::ref(*dst_sw));

	// Wait for terminating signal to be raised.
	int raised = terminating_signals.wait();

	// Stop listening and exit.
	std::cerr << strsignal(raised) << std::endl;
	terminating.set();
	pthread_kill(capture_thread.native_handle(), SIGALRM);
	capture_thread.join();
}
