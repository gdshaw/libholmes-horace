// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SYSLOG_UDP_EVENT_READER
#define LIBHOLMES_HORACE_SYSLOG_UDP_EVENT_READER

#include <memory>

#include "horace/socket_descriptor.h"
#include "horace/record.h"
#include "horace/log_record_builder.h"
#include "horace/event_reader.h"

namespace horace {

class session_builder;
class syslog_udp_endpoint;

/** A class for reading events from a Syslog UDP socket. */
class syslog_udp_event_reader:
	public event_reader {
private:
	/** The endpoint from which to capture. */
	const syslog_udp_endpoint* _ep;

	/** The channel number to use for captured events. */
	int _channel;

	/** The socket for capturing messages. */
	socket_descriptor _sock;

	/** A buffer for receiving datagram content. */
	std::unique_ptr<char[]> _buffer;

	/** An iovec structure for receiving datagrams. */
	struct iovec _iov[1];

	/** A msghdr structure for receiving datagrams. */
	struct msghdr _message;

	/** A builder for log records. */
	std::unique_ptr<log_record_builder> _builder;
public:
	/** Construct syslog event reader.
	 * @param ep the endpoint to read from
	 */
	explicit syslog_udp_event_reader(const syslog_udp_endpoint& ep,
		session_builder& builder);

	virtual const record& read();
};

} /* namespace horace */

#endif
