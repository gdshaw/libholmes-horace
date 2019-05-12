// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_ENDPOINT
#define LIBHOLMES_HORACE_MONGODB_ENDPOINT

#include "horace/endpoint.h"
#include "horace/session_writer_endpoint.h"

namespace horace {

/** An endpoint class to represent a MongoDB database. */
class mongodb_endpoint:
	public endpoint,
	public session_writer_endpoint {
public:
	/** Construct MongoDB endpoint.
	 * @param name the name of this endpoint
	 */
	mongodb_endpoint(const std::string& name);

	virtual std::unique_ptr<session_writer> make_session_writer(
		const std::string& source_id);
};

} /* namespace horace */

#endif
