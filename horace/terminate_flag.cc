// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include "horace/libc_error.h"
#include "horace/terminate_flag.h"

namespace horace {

terminate_flag::terminate_flag():
	_terminating(false) {

	if (pipe(_pipefd) == -1) {
		throw libc_error();
	}
	_nonblock(_pipefd[0]);
	_nonblock(_pipefd[1]);
}

void terminate_flag::_nonblock(int fd) {
	int flags = ::fcntl(fd, F_GETFL, 0);
	if (flags == -1) {
		throw libc_error();
	}
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) == -1) {
		throw libc_error();
	}
}

terminate_flag& terminate_flag::operator=(bool terminating) {
	bool changed = terminating != _terminating;
	_terminating = terminating;

	if (changed) {
		if (terminating) {
			size_t count = write(_pipefd[1], "", 1);
		} else {
			char buffer;
			while (read(_pipefd[0], &buffer,
				sizeof(buffer)) > 0) {}
		}
	}
	return *this;
}

int terminate_flag::poll(int fd, int events, int timeout) const {
	struct pollfd fds[2] = {{0}};
	fds[0].fd = _pipefd[0];
	fds[0].events = POLLIN;
	fds[1].fd = fd;
	fds[1].events = events;

	while (::poll(fds, 2, timeout) == -1) {
		if (errno != EINTR) {
			throw libc_error();
		}
	}
	if (fds[0].revents & POLLIN) {
		throw terminate_exception();
	}
	return fds[1].revents;
}

void terminate_flag::millisleep(int timeout) const {
	struct pollfd fds[1] = {{0}};
	fds[0].fd = _pipefd[0];
	fds[0].events = POLLIN;

	if (::poll(fds, 1, timeout) == -1) {
		if (errno != EINTR) {
			throw libc_error();
		}
	}
	if (fds[0].revents & POLLIN) {
		throw terminate_exception();
	}
}

terminate_flag terminating;

} /* namespace horace */
