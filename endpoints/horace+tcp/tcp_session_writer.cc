// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <poll.h>

#include "horace/terminate_exception.h"
#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/terminate_flag.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/address_info.h"
#include "horace/record.h"

#include "tcp_endpoint.h"
#include "tcp_session_writer.h"

namespace horace {

tcp_session_writer::tcp_session_writer(tcp_endpoint& dst_ep,
	const std::string& srcid):
	_dst_ep(&dst_ep),
	simple_session_writer(srcid) {}

void tcp_session_writer::_open() {
	// Look up the hostname and portname.
	// A fresh lookup is performed before each connection attempt in
	// order to ensure that any retries are responsive to changes to
	// the content or availability of the DNS.
	bool connected = false;
	bool logged = false;
	while (!connected) {
		try {
			address_info ai(_dst_ep->hostname(),
				_dst_ep->portname(), SOCK_STREAM, false);
			_fd = ai.make_socket();
			_fd.interruptible(true);
			ai.connect(_fd);
			_fd.wait(POLLOUT);
			connected = true;

			if (log->enabled(logger::log_notice)) {
				log_message msg(*log, logger::log_notice);
				msg << "connection established to " <<
					_dst_ep->hostname() << ":" <<
					_dst_ep->portname();
			}
		} catch (terminate_exception&) {
			throw;
		} catch (std::exception& ex) {
			if (!logged) {
				if (log->enabled(logger::log_warning)) {
					log_message msg(*log, logger::log_warning);
					msg << "failed to establish connection to " <<
						_dst_ep->hostname() << ":" <<
						_dst_ep->portname() <<
						" (will retry)";
				}
				logged = true;
			}

			terminating.millisleep(_dst_ep->retry() * 1000);
		}
	}

	_fdow = file_octet_writer(_fd);
	_fdor = file_octet_reader(_fd);
}

bool tcp_session_writer::writable() {
	return true;
}

void tcp_session_writer::handle_session_start(const record& srec) {
	if (!_fd) {
		_open();
	}
	srec.write(_fdow);
}

void tcp_session_writer::handle_session_end(const record& erec) {
	erec.write(_fdow);
}

void tcp_session_writer::handle_sync(const record& crec) {
	if (!_dst_ep->diode()) {
		crec.write(_fdow);
		simple_session_writer::read();
	}
}

void tcp_session_writer::handle_signature(const record& grec) {
	grec.write(_fdow);
}

void tcp_session_writer::handle_control(const record& rec) {
	rec.write(_fdow);
}

void tcp_session_writer::handle_event(const record& rec) {
	rec.write(_fdow);
}

bool tcp_session_writer::readable() {
	if (!_fd) {
		_open();
	}
	return _fd.ready(POLLIN) & POLLIN;
}

std::unique_ptr<record> tcp_session_writer::read() {
	if (_dst_ep->diode()) {
		return simple_session_writer::read();
	} else {
		_fdow.flush();
		return std::make_unique<record>(_session, _fdor);
	}
}

} /* namespace horace */
