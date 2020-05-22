// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sys/socket.h>

#include "horace/terminate_exception.h"
#include "horace/record.h"

#include "tcp_endpoint.h"
#include "tcp_session_reader.h"

namespace horace {

tcp_session_reader::tcp_session_reader(tcp_endpoint& src_ep,
	socket_descriptor&& fd):
	_src_ep(&src_ep),
	_fd(std::move(fd)),
	_fdor(_fd),
	_fdow(_fd) {

	_fd.interruptible(true);
}

std::unique_ptr<record> tcp_session_reader::read() {
	return std::make_unique<record>(_session, _fdor);
}

void tcp_session_reader::write(const record& rec) {
	rec.write(_fdow);
	_fdow.flush();

	if (rec.channel_number() == channel_error) {
		_fd.shutdown(SHUT_WR);
		bool done = false;
		while (!done) {
			try {
				auto rec = std::make_unique<record>(_session, _fdor);
			} catch (terminate_exception&) {
				throw;
			} catch (std::exception&) {
				done = true;
			}
		}
	}
}

} /* namespace horace */
