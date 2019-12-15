// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_LISTENER_ENDPOINT
#define LIBHOLMES_HORACE_SESSION_LISTENER_ENDPOINT

#include <memory>

namespace horace {

class session_listener;

/** An endpoint mixin class to represent a source of an arbitrary number
 * of concurrent sessions.
 * This class is intended for use by endpoints which may be required to
 * handle sessions originating from multiple concurrent sources. An
 * example would be a server socket to which multiple HORACE clients can
 * connect.
 *
 * It provides a single member function which allows a session listener
 * to be created. That in turn allows sessions to be accepted from the
 * endpoint.
 */
class session_listener_endpoint {
public:
	/** Make a listener for this endpoint.
	 * @return the resulting listener
	 */
	virtual std::unique_ptr<session_listener> make_session_listener() = 0;
};

} /* namespace horace */

#endif
