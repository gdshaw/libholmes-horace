// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_OCTET_WRITER
#define LIBHOLMES_HORACE_FILE_OCTET_WRITER

#include "horace/file_descriptor.h"
#include "horace/octet_writer.h"

namespace horace {

/** An octet writer class for writing to a file descriptor. */
class file_octet_writer:
	public octet_writer {
private:
	/** A buffer for the octet writer. */
	char _buffer[0x40];

	/** The file descriptor to be written to. */
	file_descriptor* _fd;
protected:
	virtual void _write_direct(const void* buf, size_t nbyte);
public:
	/** Construct file octet writer.
	 * @param fd the file descriptor to be written to
	 */
	file_octet_writer(file_descriptor& fd);

	virtual ~file_octet_writer();
};

} /* namespace horace */

#endif
