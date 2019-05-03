// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EVENT_READER_ENDPOINT
#define LIBHOLMES_HORACE_EVENT_READER_ENDPOINT

#include "horace/event_reader.h"

namespace horace {

/** An endpoint mixin class to represent a source of events.
 * This class is intended for use in endpoints which act as a source of
 * newly-captured events which have not yet been packaged into sessions
 * (and which can therefore be mixed with events from other endpoints).
 *
 * It provides a single member function which allows an event reader to
 * be created. That in turn allows events to be read from the endpoint.
 */
class event_reader_endpoint {
public:
	/** Make an event reader for this endpoint.
	 * @return the resulting event reader
	 */
	virtual std::unique_ptr<event_reader> make_event_reader() = 0;
};

} /* namespace horace */

#endif
