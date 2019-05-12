// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/address_info.h"
#include "horace/record.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"

#include "tcp_endpoint.h"
#include "tcp_session_writer.h"

namespace horace {

tcp_session_writer::tcp_session_writer(tcp_endpoint& dst_ep,
	const std::string& source_id):
	_dst_ep(&dst_ep),
	session_writer(source_id) {}

void tcp_session_writer::_open() {
	// Look up the hostname and portname.
	// A fresh lookup is performed before each connection attempt in
	// order to ensure that any retries are responsive to changes to
	// the content or availability of the DNS.
	address_info ai(_dst_ep->hostname(), _dst_ep->portname(), false);
	_fd = ai.make_socket();
	ai.connect(_fd);
	_fdow = file_octet_writer(_fd);
}

void tcp_session_writer::handle_session_start(const session_start_record& srec) {
	if (!_fd) {
		_open();
	}
	srec.write(_fdow);
}

void tcp_session_writer::handle_session_end(const session_end_record& erec) {
	if (!_fd) {
		_open();
	}
	erec.write(_fdow);
}

void tcp_session_writer::handle_event(const record& rec) {
	if (!_fd) {
		_open();
	}
	rec.write(_fdow);
}

} /* namespace horace */
