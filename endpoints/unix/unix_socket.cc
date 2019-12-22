// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "horace/libc_error.h"
#include "horace/log_message.h"
#include "horace/logger.h"
#include "horace/packet_record_builder.h"

#include "unix_socket.h"

namespace horace {

unix_socket::unix_socket(packet_record_builder& builder, size_t snaplen):
	socket_descriptor(AF_UNIX, SOCK_DGRAM, 0),
	_builder(&builder) {

	interruptible(true);

	// Initialse buffers for recvmsg.
	_snaplen = snaplen;
	_buffer = std::make_unique<char[]>(snaplen);
	_iov[0].iov_base = _buffer.get();
	_iov[0].iov_len = snaplen;

	// Initialise message structure for recvmsg.
	_message.msg_name = 0;
	_message.msg_namelen = 0;
	_message.msg_iov = _iov;
	_message.msg_iovlen = 1;
	_message.msg_control = 0;
	_message.msg_controllen = 0;

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "opened UNIX domain socket (" <<
			"snaplen=" << snaplen << ")";
	}
}

unix_socket::~unix_socket() {
	if (!_pathname.empty()) {
		unlink(_pathname.c_str());
	}
}

void unix_socket::bind(const std::string& pathname) {
	// Construct address.
	struct sockaddr_un addr = {0};
	addr.sun_family = AF_UNIX;
	if (snprintf(addr.sun_path, sizeof(addr.sun_path), "%s",
		pathname.c_str()) >= sizeof(addr.sun_path)) {

		throw libc_error(ENAMETOOLONG);
	}

	// Determine whether a socket exists with the requested pathname.
	struct stat statbuf;
	lstat(pathname.c_str(), &statbuf);
	if (S_ISSOCK(statbuf.st_mode)) {
		// If it exists, attempt to connect to it.
		try {
			connect(addr);
			throw libc_error(EADDRINUSE);
		} catch (libc_error& ex) {
			if (ex.errno_value() == ECONNREFUSED) {
				// If it exists as a socket, but refuses
				// connections, take that as sufficient
				// evidence that it is safe to unlink.
				// (No error detection here, since the
				// error returned when binding fails will
				// be more informative.)
				unlink(pathname.c_str());
			}
		}
	}

	socket_descriptor::bind(addr);
	_pathname = pathname;
}

const record& unix_socket::read() {
	// If there are any records remaining from the last call to this
	// function then return the next one (which should never happen in
	// this instance, but included here because this is how the packet
	// record builder is intended to be used).
	if (const record* rec = _builder->next()) {
		return *rec;
	}

	// Read a message from the socket, with the MSG_TRUNC flag set in
	// order to return the original length of the packet as opposed to
	// the (potentially truncated) captured length.
	struct msghdr message = _message;
	size_t pkt_origlen = recvmsg(&message, MSG_TRUNC);

	// If the packet was not truncated then the captured length will be
	// equal to the original length, otherwise it will be equal to the
	// configured snaplen.
	size_t pkt_snaplen = (message.msg_flags & MSG_TRUNC) ?
		_snaplen : pkt_origlen;

	struct timespec ts;
	if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
		throw libc_error();
	}

	_builder->build_packet(&ts, _buffer.get(), pkt_snaplen, pkt_origlen, 0);
	return *_builder->next();
}

} /* namespace horace */
