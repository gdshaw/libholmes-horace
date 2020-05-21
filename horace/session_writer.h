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

	/** Test whether the endpoint is writable.
	 * Writable means that records can be freely written to the
	 * endpoint with a good prospect that they will be accepted.
	 *
	 * If the endpoint ceases to be writable during event capture
	 * then the session should be ended, and capture suspended unless
	 * and until the endpoint becomes writable again.
	 *
	 * Sessions should be ended promptly but gracefully. To facilitate
	 * this, it is permissible for a limited number of records to be
	 * written after the endpoint becomes unwritable, including event
	 * records if that helps to establish an accurate end time. The
	 * endpoint should accept such records if it is able to (or throw
	 * an exception if it cannot).
	 *
	 * Interruptions should be reported only if they are serious enough
	 * to justify ending the session. Examples might include:
	 * - Insufficient free space for writing to spoolfiles.
	 * - Inability to connect to a remote server.
	 *
	 * If there is a need for writeability tests to be rate-limited then
	 * that is the responsibility of the session writer.
	 * @return true if writable, otherwise false
	 */
	virtual bool writable() = 0;

	/** Write a record to the endpoint.
	 * @param rec the record to be written
	 */
	virtual void write(const record& rec) = 0;

	/** Test whether the endpoint is readable.
	 * Readable means that at least part of a record is available for
	 * reading. It does not necessarily imply that a complete record can
	 * be read without blocking. (However, once part of a record has been
	 * seen, either the remainder of that record or an end of file
	 * condition should follow promptly under normal circumstances.)
	 * @return true if readable, otherwise false
	 */
	virtual bool readable() = 0;

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
