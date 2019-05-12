// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <poll.h>
#include <sys/socket.h>

#include "horace/libc_error.h"
#include "horace/filter.h"
#include "horace/socket_descriptor.h"

namespace horace {

socket_descriptor::socket_descriptor(int domain, int type, int protocol):
	file_descriptor(::socket(domain, type, protocol)) {

	if (*this == -1) {
		throw libc_error();
	}
}

void socket_descriptor::bind(const struct sockaddr* addr, socklen_t addrlen) {
	if (::bind(*this, addr, addrlen) == -1) {
		throw horace::libc_error();
	}
}

void socket_descriptor::listen(int backlog) {
	if (::listen(*this, backlog) == -1) {
		throw libc_error();
	}
}

socket_descriptor socket_descriptor::accept(struct sockaddr* addr, socklen_t addrlen) {
	int fd = -1;
	while (fd == -1) {
		fd = ::accept(*this, 0, 0);
		if (fd == -1) {
			if ((errno == EWOULDBLOCK) || (errno == EAGAIN)) {
				wait(POLLIN);
			} else {
				throw libc_error();
			}
		}
	}
	return socket_descriptor(fd);
}

void socket_descriptor::connect(const struct sockaddr* addr, socklen_t addrlen) {
	if (::connect(*this, addr, addrlen) == -1) {
		if (errno == EINPROGRESS) {
			wait(POLLOUT);
		} else {
			throw horace::libc_error();
		}
	}
}

size_t socket_descriptor::recvmsg(struct msghdr* message, int flags) {
	ssize_t count = -1;
	while (count < 0) {
		count = ::recvmsg(*this, message, flags);
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

void socket_descriptor::setsockopt(int level, int optname,
	const void* optval, socklen_t optlen) {

	if (::setsockopt(*this, level, optname, optval, optlen) == -1) {
		throw libc_error();
	}
}

void socket_descriptor::attach(const filter& filt) {
	const struct sock_fprog* fprog = filt.compile();
	setsockopt(SOL_SOCKET, SO_ATTACH_FILTER, *fprog);
}

} /* namespace horace */
