// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "horace/libc_error.h"
#include "horace/file_descriptor.h"

namespace horace {

file_descriptor::file_descriptor(const std::string& pathname, int flags,
	int mode):
	_fd(open(pathname.c_str(), flags, mode)) {

	if (_fd == -1) {
		throw libc_error();
	}
}

file_descriptor::~file_descriptor() {
	if (_fd != -1) {
		::close(_fd);
	}
}

void file_descriptor::wait(int events) const {
	struct pollfd fds[1] = {{0}};
	fds[0].fd = _fd;
	fds[0].events = events;

	if (ppoll(fds, 1, 0, 0) == -1) {
		if (errno != EINTR) {
			throw libc_error();
		}
	}
}

size_t file_descriptor::read(void* buf, size_t nbyte) {
	ssize_t count = -1;
	while (count < 0) {
		count = ::read(_fd, buf, nbyte);
		if (count == -1) {
			if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
				wait(POLLIN);
			} else {
				throw libc_error();
			}
		}
	}
	return count;
}

void file_descriptor::write(const void* buf, size_t nbyte) {
	const char* ptr = static_cast<const char*>(buf);
	while (nbyte > 0) {
		ssize_t count = ::write(_fd, ptr, nbyte);
		if (count == -1) {
			if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
				wait(POLLOUT);
			} else {
				throw libc_error();
			}
		}
		ptr += count;
		nbyte -= count;
	}
}

} /* namespace horace */
