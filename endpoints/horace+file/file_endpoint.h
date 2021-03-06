// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_ENDPOINT
#define LIBHOLMES_HORACE_FILE_ENDPOINT

#include <string>

#include "horace/file_descriptor.h"
#include "horace/endpoint.h"
#include "horace/session_listener_endpoint.h"
#include "horace/session_writer_endpoint.h"

#include "free_space_checker.h"

namespace horace {

/** An endpoint class to represent a set of HORACE filestores. */
class file_endpoint:
	public endpoint,
	public session_listener_endpoint,
	public session_writer_endpoint {
private:
	/** The pathname for this endpoint. */
	std::string _pathname;

	/** A file descriptor for synchronising the directory. */
	file_descriptor _fd;

	/** The size to which spoolfiles are permitted to grow, in octets. */
	size_t _filesize;

	/** True if deletion of spoolfiles should be suppressed, otherwise false. */
	bool _nodelete;

	/** An optional object for checking free space thresholds. */
	std::unique_ptr<free_space_checker> _fschecker;
public:
	/** Construct file endpoint.
	 * @param name the name of this endpoint
	 */
	explicit file_endpoint(const std::string& name);

	/** Get the pathname.
	 * @return the pathname
	 */
	const std::string& pathname() const {
		return _pathname;
	}

	/** Get the size to which spoolfiles are permitted to grow.
	 * @return the size, in octets
	 */
	size_t filesize() const {
		return _filesize;
	}

	/** Check whether deletion of spoolfiles should be suppressed
	 * @return true to suppress, otherwise false
	 */
	bool nodelete() const {
		return _nodelete;
	}

	/** Test whether the endpoint is writable.
	 * This function should have the same behaviour as
	 * horace::session_writer::writable.
	 * @return true if writable, otherwise false
	 */
	bool writable();

	virtual std::unique_ptr<session_listener> make_session_listener();
	virtual std::unique_ptr<session_writer> make_session_writer(
		const std::string& srcid);
};

} /* namespace horace */

#endif
