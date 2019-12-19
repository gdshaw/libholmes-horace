// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>

#include <signal.h>

#include "horace/terminate_exception.h"
#include "horace/terminate_flag.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/record.h"
#include "horace/event_source.h"
#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"
#include "horace/new_session_writer.h"

namespace horace {

void event_source::_capture() {
	try {
		while (true) {
			// Ensure that termination is picked up, even if
			// the thread never blocks.
			terminating.poll();

			// Read record from source, write to destination.
			const record& rec = _er->read();
			_nsw->write_event(rec);
		}
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

void event_source::_do_capture(event_source& es) {
	es._capture();
}

event_source::event_source(endpoint& ep, new_session_writer& nsw,
	session_builder& sb):
	_ep(dynamic_cast<event_reader_endpoint*>(&ep)),
	_nsw(&nsw) {

	_er = _ep->make_event_reader(sb);
}

void event_source::start() {
	_thread = std::thread(_do_capture, std::ref(*this));
}

void event_source::stop() {
	pthread_kill(_thread.native_handle(), SIGALRM);
	_thread.join();
}

void event_source::attach(const filter& filt) {
	_er->attach(filt);
}

} /* namespace horace */
