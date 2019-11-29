// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_OCTET_WRITER
#define LIBHOLMES_HORACE_FILE_OCTET_WRITER

#include "horace/buffer.h"
#include "horace/octet_writer.h"

namespace horace {

class file_descriptor;

/** An octet writer class for writing to a file descriptor. */
class file_octet_writer:
	public buffer,
	public octet_writer {
private:
	/** The file descriptor to be written to. */
	file_descriptor* _fd;
protected:
	virtual void _write_direct(const void* buf, size_t nbyte);
public:
	/** Construct file octet writer without a file descriptor. */
	file_octet_writer() = default;

	/** Construct file octet writer.
	 * A larger buffer can be expected to reduce the number of system
	 * calls performed, but increase the amount of copying (due to fewer
	 * direct writes).
	 * @param fd the file descriptor to be written to
	 * @param bufsize the required buffer size, in octets
	 */
	explicit file_octet_writer(file_descriptor& fd,
		size_t bufsize = 0x40);

	file_octet_writer(const file_octet_writer& that) = delete;
	file_octet_writer& operator=(const file_octet_writer& that) = delete;

	file_octet_writer(file_octet_writer&& that) = default;
	file_octet_writer& operator=(file_octet_writer&& that) = default;

	virtual ~file_octet_writer();
};

} /* namespace horace */

#endif
