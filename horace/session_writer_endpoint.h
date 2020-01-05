// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_WRITER_ENDPOINT
#define LIBHOLMES_HORACE_SESSION_WRITER_ENDPOINT

#include <memory>

namespace horace {

class session_writer;

/** An endpoint mixin class to represent a destination to which multiple
 * concurrent sessions can be sent.
 * This class is intended for use in endpoints which handle inbound
 * connections. An example would be a server socket to which multiple
 * HORACE clients can connect.
 *
 * It provides a single member function which allows a session listener
 * to be created. That in turn allows sessions to be accepted from the
 * endpoint.
 */
class session_writer_endpoint {
public:
	/** Make a session writer for this endpoint.
	 * @param srcid the source ID for the session writer
	 * @return the resulting session writer
	 */
	virtual std::unique_ptr<session_writer> make_session_writer(
		const std::string& srcid) = 0;
};

} /* namespace horace */

#endif
