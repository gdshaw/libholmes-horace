// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_LISTENER_ENDPOINT
#define LIBHOLMES_HORACE_SESSION_LISTENER_ENDPOINT

#include "horace/session_listener.h"

namespace horace {

/** An endpoint mixin class to represent an endpoint which can listen for
 * and accept multiple concurrent sessions. */
class session_listener_endpoint {
public:
	/** Make a listener for this endpoint.
	 * @return the resulting listener
	 */
	virtual std::unique_ptr<session_listener> make_session_listener() = 0;
};

} /* namespace horace */

#endif
