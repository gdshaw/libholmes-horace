// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/record_builder.h"

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
	record_builder builder(_fdor);
	return builder.build();
}

void tcp_session_reader::write(const record& rec) {
	rec.write(_fdow);
	_fdow.flush();
}

} /* namespace horace */
