// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_WRITER
#define LIBHOLMES_HORACE_SESSION_WRITER

#include <memory>
#include <string>

#include "horace/source_id.h"

namespace horace {

class record;

/** A class for writing sessions to an endpoint. */
class session_writer {
private:
	/** The source ID. */
	source_id _srcid;
public:
	/** Construct session writer.
	 * @param srcid the required source ID
	 */
	explicit session_writer(const std::string& srcid);

	virtual ~session_writer() = default;

	/** Get the source ID. */
	const std::string& srcid() const {
		return _srcid;
	}

	/** Write a record to the endpoint.
	 * @param rec the record to be written
	 */
	virtual void write(const record& rec) = 0;

	/** Read a record from the endpoint.
	 * If there is no record immediately available then this function
	 * will block until one can be read, or until there is no further
	 * prospect of that happening.
	 * @return the resulting record
	 */
	virtual std::unique_ptr<record> read() = 0;
};

} /* namespace horace */

#endif
