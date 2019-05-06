// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>

#include "horace/file_octet_writer.h"

namespace horace {

void file_octet_writer::_write_direct(const void* buf, size_t nbyte) {
	_fd->write(buf, nbyte);
}

file_octet_writer::file_octet_writer(file_descriptor& fd):
	buffer(0x40),
	octet_writer(buffer_ptr(), buffer_size()),
	_fd(&fd) {}

file_octet_writer::~file_octet_writer() {
	try {
		flush();
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

} /* namespace horace */
