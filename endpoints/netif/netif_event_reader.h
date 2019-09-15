// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NETIF_EVENT_READER
#define LIBHOLMES_HORACE_NETIF_EVENT_READER

#include <memory>

#include "horace/event_reader.h"

#include "basic_packet_socket.h"

namespace horace {

class netif_endpoint;

/** A class for reading events from a network interface endpoint. */
class netif_event_reader:
	public event_reader {
private:
	/** The endpoint from which to capture. */
	const netif_endpoint* _ep;

	/** The channel number to use for captured events. */
	int _channel;

	/** The socket for capturing packets. */
	std::unique_ptr<basic_packet_socket> _sock;
public:
	/** Construct network interface event reader.
	 * @param ep the endpoint to read from
	 */
	explicit netif_event_reader(const netif_endpoint& ep);

	virtual const record& read();
	virtual void attach(const filter& filt);
	virtual void build_session(session_builder& builder);
};

} /* namespace horace */

#endif
