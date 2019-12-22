// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNIX_EVENT_READER
#define LIBHOLMES_HORACE_UNIX_EVENT_READER

#include "horace/event_reader.h"
#include "horace/packet_record_builder.h"

#include "unix_socket.h"

namespace horace {

class session_builder;
class unix_endpoint;

/** A class for reading events from a UNIX-domain socket. */
class unix_event_reader:
	public event_reader {
private:
	/** The endpoint from which to capture. */
	const unix_endpoint* _ep;

	/** The channel number to use for captured events. */
	int _channel;

	/** A builder for making packet records. */
	std::unique_ptr<packet_record_builder> _builder;

	/** The socket for capturing messages. */
	std::unique_ptr<unix_socket> _sock;
public:
	/** Construct UNIX-domain event reader.
	 * @param ep the endpoint to read from
	 */
	explicit unix_event_reader(const unix_endpoint& ep,
		session_builder& builder);

	virtual const record& read();
};

} /* namespace horace */

#endif
