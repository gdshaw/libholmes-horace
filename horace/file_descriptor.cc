// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/file.h>
#include <sys/socket.h>

#include "horace/libc_error.h"
#include "horace/terminate_flag.h"
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

bool file_descriptor::interruptible() const {
	return ::fcntl(*this, F_GETFL, 0) & O_NONBLOCK;
}

void file_descriptor::interruptible(bool enable) {
	int flags = ::fcntl(*this, F_GETFL, 0);
	if (flags == -1) {
		throw libc_error();
	}
	if (enable) {
		flags |= O_NONBLOCK;
	} else {
		flags &= ~O_NONBLOCK;
	}
	if (::fcntl(*this, F_SETFL, flags) == -1) {
		throw libc_error();
	}
}

int file_descriptor::wait(int events, int timeout) const {
	int revents = terminating.poll(_fd, events, timeout);
	if (revents & POLLERR) {
		handle_pollerror();
	}
	return revents;
}

int file_descriptor::ready(int events) const {
	struct pollfd fds[1] = {{0}};
	fds[0].fd = _fd;
	fds[0].events = events;

	while (::poll(fds, 1, 0) == -1) {
		if (errno != EINTR) {
			throw libc_error();
		}
	}
	return fds[0].revents;
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

void file_descriptor::fsync() const {
	if (::fsync(_fd) == -1) {
		throw libc_error();
	}
}

bool file_descriptor::lock() {
	if (::flock(_fd, LOCK_EX|LOCK_NB) == -1) {
		if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
			return false;
		} else {
			throw libc_error();
		}
	}
	return true;
}

void file_descriptor::unlock() {
	if (::flock(_fd, LOCK_UN) == -1) {
		throw libc_error();
	}
}


} /* namespace horace */
