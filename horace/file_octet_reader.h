// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_OCTET_READER
#define LIBHOLMES_HORACE_FILE_OCTET_READER

#include "horace/file_descriptor.h"
#include "horace/octet_reader.h"

namespace horace {

/** An octet reader class for reading from a file descriptor. */
class file_octet_reader:
	public octet_reader {
private:
	/** A buffer for the octet reader. */
	char buffer[0x40];

	/** The file descriptor to be read from. */
	file_descriptor* _fd;
protected:
	virtual size_t _read_direct(void* buf, size_t nbyte);
public:
	/** Construct file octet reader.
	 * @param fd the file descriptor to be read from
	 */
	file_octet_reader(file_descriptor& fd);
};

} /* namespace horace */

#endif
