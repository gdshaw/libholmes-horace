// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NETIF_EVENT_READER
#define LIBHOLMES_HORACE_NETIF_EVENT_READER

#include <memory>

#include "basic_packet_socket.h"
#include "horace/event_reader.h"

namespace horace {

/** A class for reading events from a network interface endpoint. */
class netif_event_reader:
	public event_reader {
private:
	/** The socket for capturing packets. */
	std::unique_ptr<basic_packet_socket> _sock;
public:
	/** Construct network interface event reader. */
	netif_event_reader();

	virtual const record& read();
};

} /* namespace horace */

#endif
