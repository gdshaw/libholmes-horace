// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <poll.h>

#include "horace/libc_error.h"
#include "horace/terminate_flag.h"

namespace horace {

terminate_flag::terminate_flag():
	_terminating(false) {

	if (pipe(_pipefd) == -1) {
		throw libc_error();
	}
}

void terminate_flag::set() {
	_terminating = true;
	size_t count = write(_pipefd[1], "", 1);
}

int terminate_flag::poll(int fd, int events) const {
	struct pollfd fds[2] = {{0}};
	fds[0].fd = _pipefd[0];
	fds[0].events = POLLIN;
	fds[1].fd = fd;
	fds[1].events = events;

	while (::poll(fds, 2, -1) == -1) {
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
	fds[0].fd = POLLIN;

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
