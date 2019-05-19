// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>

#include "horace/libc_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/record_builder.h"

#include "spoolfile_reader.h"
#include "file_session_reader.h"

namespace horace {

size_t spoolfile_reader::_read_direct(void* buf, size_t nbyte) {
	bool eof = false;
	while (true) {
		size_t count = ::read(_fd, buf, nbyte);
		if (count > 0) {
			return count;
		} else if (eof) {
			// Must not return zero unless the observation of
			// _next_pathname preceded the attempt to read data.
			return 0;
		} else if (access(_next_pathname.c_str(), F_OK) == 0) {
			eof = true;
		} else {
			_fsr->wait();
		}
	}
}

spoolfile_reader::spoolfile_reader(file_session_reader& fsr,
	const std::string& pathname, const std::string& next_pathname):
	buffer(0x40),
	octet_reader(buffer_ptr(), buffer_size(), 0),
	_fsr(&fsr),
	_fd(pathname, O_RDONLY),
	_pathname(pathname),
	_next_pathname(next_pathname) {

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "opened spoolfile " << _pathname;
	}
}

std::unique_ptr<record> spoolfile_reader::read() {
	record_builder builder(*this);
	return builder.build();
}

void spoolfile_reader::unlink() {
	if (::unlink(_pathname.c_str()) == -1) {
		throw libc_error();
	}

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "deleted spoolfile " << _pathname;
	}
}

} /* namespace horace */
