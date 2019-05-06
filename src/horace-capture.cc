// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <memory>
#include <iostream>

#include <getopt.h>

#include "horace/hostname.h"
#include "horace/source_attribute.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/record.h"
#include "horace/record_builder.h"
#include "horace/event_reader.h"
#include "horace/session_writer.h"
#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"
#include "horace/session_writer_endpoint.h"

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
	horace::hostname source_id;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "+hV")) != -1) {
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
	std::unique_ptr<horace::endpoint> src_ep =
		horace::endpoint::make(argv[optind++]);

	// Make event reader for source endpoint.
	horace::event_reader_endpoint* src_erep =
		dynamic_cast<horace::event_reader_endpoint*>(src_ep.get());
	if (!src_erep) {
		std::cerr << "Source endpoint is unable to capture events."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<horace::event_reader> src_er =
		src_erep->make_event_reader();

	// Parse destination endpoint.
	if (optind == argc) {
		std::cerr << "Destination endpoint not specified."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<horace::endpoint> dst_ep =
		horace::endpoint::make(argv[optind++]);

	// Make session writer for destination endpoint.
	horace::session_writer_endpoint* dst_swep =
		dynamic_cast<horace::session_writer_endpoint*>(dst_ep.get());
	if (!dst_swep) {
		std::cerr << "Destination endpoint is unable to receive sessions."
			<< std::endl;
		exit(1);
	}
	std::unique_ptr<horace::session_writer> dst_sw =
		dst_swep->make_session_writer(source_id);

	if (optind != argc) {
		std::cerr << "Too many arguments on command line."
			<< std::endl;
	}

	horace::record_builder srecb(horace::record::REC_SESSION_START);
	srecb.append(std::make_shared<horace::source_attribute>(source_id));
	srecb.append(std::make_shared<horace::posix_timespec_attribute>());
	std::unique_ptr<horace::record> srec = srecb.build();
	dst_sw->write(*srec);

	while (true) {
		const horace::record& rec = src_er->read();
		dst_sw->write(rec);
	}
}
