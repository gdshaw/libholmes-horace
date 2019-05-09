// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/endpoint_error.h"

#include "packet_socket.h"
#include "ring_buffer_v1.h"
#include "netif_event_reader.h"
#include "netif_endpoint.h"

namespace horace {

class record;

netif_event_reader::netif_event_reader(const netif_endpoint& ep) {
	std::string method = ep.method();
	if (method == "packet") {
		_sock = std::make_unique<packet_socket>(ep.snaplen());
	} else if (method == "ringv1") {
		_sock = std::make_unique<ring_buffer_v1>(ep.snaplen(), ep.capacity());
	} else {
		throw endpoint_error("interface capture method not recognised");
	}

	if (ep.netif()) {
		_sock->bind(ep.netif());
	}

	if (ep.promiscuous()) {
		if (!ep.netif()) {
			throw endpoint_error("cannot enable promiscuous "
				"mode for all interfaces.");
		}
		_sock->set_promiscuous(ep.netif());
	}
}

const record& netif_event_reader::read() {
	return _sock->read();
};

} /* namespace horace */
