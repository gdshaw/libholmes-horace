// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_OCTET_READER
#define LIBHOLMES_HORACE_FILE_OCTET_READER

#include "horace/buffer.h"
#include "horace/octet_reader.h"

namespace horace {

class file_descriptor;

/** An octet reader class for reading from a file descriptor. */
class file_octet_reader:
	public buffer,
	public octet_reader {
private:
	/** The file descriptor to be read from. */
	file_descriptor* _fd;
protected:
	virtual size_t _read_direct(void* buf, size_t nbyte);
public:
	/** Construct file octet reader without a file descriptor. */
	file_octet_reader() = default;

	/** Construct file octet reader.
         * A larger buffer can be expected to reduce the number of system
         * calls performed, but increase the amount of copying (due to fewer
         * direct reads).
	 * @param fd the file descriptor to be read from
	 * @param bufsize the required buffer size, in octets
	 */
	explicit file_octet_reader(file_descriptor& fd,
		size_t bufsize = 0x40);
};

} /* namespace horace */

#endif
