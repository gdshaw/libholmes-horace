// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SPOOLFILE_READER
#define LIBHOLMES_HORACE_SPOOLFILE_READER

#include "horace/file_descriptor.h"
#include "horace/buffer.h"
#include "horace/octet_reader.h"
#include "horace/record.h"

namespace horace {

class file_session_reader;

/** An octet reader class for reading from a spoolfile. */
class spoolfile_reader:
	public buffer,
	public octet_reader {
private:
	/** The file session reader which is reading the spoolfile. */
	file_session_reader* _fsr;

	/** The file descriptor to be read from. */
	file_descriptor _fd;

	/** The pathname of this spoolfile. */
	std::string _pathname;

	/** The pathname of the following spoolfile. */
	std::string _next_pathname;
protected:
	virtual size_t _read_direct(void* buf, size_t nbyte);
public:
	/** Construct spoolfile reader.
	 * @param fsr the file session reader which is reading the spoolfile
	 * @param pathname the pathname of the spoolfile to be read
	 * @param next_pathname the pathname of the following spoolfile
	 */
	explicit spoolfile_reader(file_session_reader& fsr,
		const std::string& pathname, const std::string& next_pathname);

	/** Attempt to read record from spoolfile.
	 * If a record cannot be read immediately then this function will
	 * block until it is able to do so, or until there is no further
	 * prospect of that happening in which case an appropriate exception
	 * will be thrown
	 * @return the record
	 */
	std::unique_ptr<record> read();

	/** Get the pathname of this spoolfile.
	 * @return the pathname
	 */
	const std::string& pathname() {
		return _pathname;
	}

	/** Get the pathname of the following spoolfile.
	 * @return the pathname
	 */
	const std::string& next_pathname() {
		return _next_pathname;
	}
};

} /* namespace horace */

#endif
