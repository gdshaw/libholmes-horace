// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>

#include "horace/libc_error.h"
#include "horace/file_octet_reader.h"

namespace horace {

size_t file_octet_reader::_read_direct(void* buf, size_t nbyte) {
	return _fd->read(buf, nbyte);
}

file_octet_reader::file_octet_reader(file_descriptor& fd):
	octet_reader(buffer, sizeof(buffer), 0),
	_fd(&fd) {}

} /* namespace horace */
