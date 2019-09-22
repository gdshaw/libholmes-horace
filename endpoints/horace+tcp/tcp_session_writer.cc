// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <poll.h>

#include "horace/terminate_exception.h"
#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/signal_set.h"
#include "horace/terminate_flag.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/address_info.h"
#include "horace/record.h"
#include "horace/record_builder.h"

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
	bool connected = false;
	bool logged = false;
	while (!connected) {
		try {
			address_info ai(_dst_ep->hostname(),
				_dst_ep->portname(), false);
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

			struct timespec timeout = {_dst_ep->retry(), 0};
			if (ppoll(0, 0, &timeout, terminating_signals) == -1) {
				if (errno != EINTR) {
					throw libc_error();
				}
				terminating.poll();
			}
		}
	}

	_fdow = file_octet_writer(_fd);
	_fdor = file_octet_reader(_fd);
}

void tcp_session_writer::write(const record& rec) {
	if (!_fd) {
		_open();
	}
	rec.write(_fdow);
}

std::unique_ptr<record> tcp_session_writer::read() {
	_fdow.flush();
	record_builder builder(_session, _fdor);
	return builder.build();
}

} /* namespace horace */
