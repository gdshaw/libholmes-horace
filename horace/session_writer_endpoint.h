// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_WRITER_ENDPOINT
#define LIBHOLMES_HORACE_SESSION_WRITER_ENDPOINT

#include "horace/session_writer.h"

namespace horace {

/** An endpoint mixin class to represent a destination for sessions. */
class session_writer_endpoint {
public:
	/** Make a session writer for this endpoint.
	 * @param source_id the source ID for the session writer
	 * @return the resulting session writer
	 */
	virtual std::unique_ptr<session_writer> make_session_writer(
		const std::string& source_id) = 0;
};

} /* namespace horace */

#endif
