// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_LISTENER
#define LIBHOLMES_HORACE_SESSION_LISTENER

#include <memory>

#include "horace/session_reader.h"

namespace horace {

/** A class able to listen for and accept multiple concurrent sessions
 * from an endpoint. */
class session_listener {
public:
	virtual ~session_listener() = default;

	/** Accept a session from the endpoint.
	 * If there is no session immediately available then this function
	 * will block until one can be accepted, or until there is no further
	 * prospect of that happening.
	 *
	 * Depending on the endpoint implementation, it may be possible to
	 * read multiple sessions using the same reader, however if that is
	 * the case then they will all have the same source ID.
	 * @return a session reader for the accepted session
	 */
	virtual std::unique_ptr<session_reader> accept() = 0;
};

} /* namespace horace */

#endif
