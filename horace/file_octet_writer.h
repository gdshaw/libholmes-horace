// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_OCTET_WRITER
#define LIBHOLMES_HORACE_FILE_OCTET_WRITER

#include "horace/file_descriptor.h"
#include "horace/buffer.h"
#include "horace/octet_writer.h"

namespace horace {

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
	file_octet_writer() {}

	/** Construct file octet writer.
	 * @param fd the file descriptor to be written to
	 */
	file_octet_writer(file_descriptor& fd);

	file_octet_writer(const file_octet_writer& that) = delete;
	file_octet_writer& operator=(const file_octet_writer& that) = delete;

	file_octet_writer(file_octet_writer&& that) = default;
	file_octet_writer& operator=(file_octet_writer&& that) = default;

	virtual ~file_octet_writer();
};

} /* namespace horace */

#endif
