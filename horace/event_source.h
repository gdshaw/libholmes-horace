// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EVENT_SOURCE
#define LIBHOLMES_HORACE_EVENT_SOURCE

#include <thread>

#include "horace/event_reader.h"

namespace horace {

class filter;
class endpoint;
class event_reader_endpoint;
class session_builder;
class new_session_writer;

/** A class for asyncrhonously capturing events from an endpoint. */
class event_source {
private:
	/** The source endpoint, as a session_reader_endpoint. */
	event_reader_endpoint* _ep;

	/** An event reader for the source endpoint. */
	std::unique_ptr<event_reader> _er;

	/** A new_session_writer for receiving the events. */
	new_session_writer* _nsw;

	/** A thread for capturing events. */
	std::thread _thread;

	/** Capture events.
	 * @param es the event source from which to capture
	 */
	static void _capture(event_source& es);
public:
	/** Construct event source.
	 * @param ep the source endpoint
	 * @param nsw a new_session_writer for receiving the events
	 * @param sb a session builder for the current session
	 */
	event_source(endpoint& ep, new_session_writer& nsw,
		session_builder& sb);

	/** Start capturing events. */
	void start();

	/** Stop capturing events. */
	void stop();

	/** Attach a BPF filter to this event source.
	 * This will have no effect if this event source does not capture
	 * network traffic.
	 * @param filt the filter to be attached
	 */
	void attach(const filter& filt);
};

} /* namespace horace */

#endif
