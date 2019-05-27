// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_READER
#define LIBHOLMES_HORACE_SESSION_READER

#include <memory>
#include <string>

namespace horace {

class record;

/** A class for reading sessions from an endpoint. */
class session_reader {
public:
	virtual ~session_reader() = default;

	/** Read a record from the endpoint.
	 * If there is no record immediately available then this function
	 * will block until one can be read, or until there is no further
	 * prospect of that happening.
	 * @return the resulting record
	 */
	virtual std::unique_ptr<record> read() = 0;

	/** Write a record to the endpoint.
	 * @param rec the record to be written
	 */
	virtual void write(const record& rec) = 0;

	/** Attempt to reset this session reader.
	 * Session readers are not required to have a reset capability, in
	 * which case they should unconditionally return false. This is the
	 * default behaviour if the reset function is not overridden.
	 * @return true if the attempt was successful, otherwise false.
	 */
	virtual bool reset();
};

} /* namespace horace */

#endif
