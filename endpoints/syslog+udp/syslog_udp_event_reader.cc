// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/log_message.h"
#include "horace/logger.h"
#include "horace/address_info.h"
#include "horace/attribute_list.h"
#include "horace/string_attribute.h"
#include "horace/session_builder.h"

#include "syslog_udp_event_reader.h"
#include "syslog_udp_endpoint.h"
#include "syslog_error.h"
#include "syslog_parser.h"

namespace horace {

class record;

syslog_udp_event_reader::syslog_udp_event_reader(
	const syslog_udp_endpoint& ep, session_builder& session):
	_ep(&ep) {

	attribute_list attrs;
	_channel = session.define_channel("log", std::move(attrs));
	_builder = std::make_unique<log_record_builder>(session, _channel);

	address_info ai(ep.hostname(), ep.portname(), SOCK_DGRAM, true);
	_sock = ai.make_socket();
	_sock.interruptible(true);
	ai.bind(_sock);

	// Initialse buffers for recvmsg.
	// Messages of any size are supported up to the maximum UDP payload
	// length of 65535 octets.
	const size_t buffer_size = 65535;
	_buffer = std::make_unique<char[]>(buffer_size);
	_iov[0].iov_base = _buffer.get();
	_iov[0].iov_len = buffer_size;

	// Initialise message structure for recvmsg.
	_message.msg_name = 0;
	_message.msg_namelen = 0;
	_message.msg_iov = _iov;
	_message.msg_iovlen = 1;
	_message.msg_control = 0;
	_message.msg_controllen = 0;

	if (log->enabled(logger::log_notice)) {
		log_message msg(*log, logger::log_notice);
		msg << "listening for syslog on " << _ep->hostname()
			<< ":" << _ep->portname();
	}
}

const record& syslog_udp_event_reader::read() {
	struct msghdr message = _message;
	size_t count = _sock.recvmsg(&message, 0);

	syslog_parser parser(_buffer.get(), count);
	parser.read_message(*_builder);
	return _builder->build();
};

} /* namespace horace */
