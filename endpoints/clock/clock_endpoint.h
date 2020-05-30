// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_CLOCK_ENDPOINT
#define LIBHOLMES_HORACE_CLOCK_ENDPOINT

#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"

namespace horace {

/** An endpoint class for monitoring the system clock. */
class clock_endpoint:
	public endpoint,
	public event_reader_endpoint {
public:
	/** Construct clock endpoint.
	 * @param name the name of this endpoint
	 */
	explicit clock_endpoint(const std::string& name);

	virtual std::unique_ptr<event_reader> make_event_reader(
		session_builder& session);
};

} /* namespace horace */

#endif
