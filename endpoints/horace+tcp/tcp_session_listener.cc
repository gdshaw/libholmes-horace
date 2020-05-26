// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/address_info.h"

#include "tcp_endpoint.h"
#include "tcp_session_reader.h"
#include "tcp_session_listener.h"

namespace horace {

tcp_session_listener::tcp_session_listener(tcp_endpoint& src_ep):
	_src_ep(&src_ep) {

	address_info ai(src_ep.hostname(), src_ep.portname(),
		SOCK_STREAM, true);
	_fd = ai.make_socket();
	_fd.interruptible(true);
	_fd.setsockopt<int>(SOL_SOCKET, SO_REUSEADDR, 1);
	ai.bind(_fd);
	_fd.listen();
}

std::unique_ptr<session_reader> tcp_session_listener::accept() {
	socket_descriptor srfd = _fd.accept(0, 0);
	return std::make_unique<tcp_session_reader>(*_src_ep, std::move(srfd));
}

} /* namespace horace */
