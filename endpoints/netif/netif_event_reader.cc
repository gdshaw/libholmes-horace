// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/endpoint_error.h"
#include "horace/netif_attribute.h"

#include "packet_socket.h"
#include "ring_buffer_v1.h"
#include "ring_buffer_v2.h"
#include "ring_buffer_v3.h"
#include "netif_event_reader.h"
#include "netif_endpoint.h"

namespace horace {

class record;

netif_event_reader::netif_event_reader(const netif_endpoint& ep):
	_ep(&ep) {

	std::string method = ep.method();
	if (method == "packet") {
		_sock = std::make_unique<packet_socket>(ep.snaplen(), ep.capacity());
	} else if (method == "ringv1") {
		_sock = std::make_unique<ring_buffer_v1>(ep.snaplen(), ep.capacity());
	} else if (method == "ringv2") {
		_sock = std::make_unique<ring_buffer_v2>(ep.snaplen(), ep.capacity());
	} else if (method == "ringv3") {
		_sock = std::make_unique<ring_buffer_v3>(ep.snaplen(), ep.capacity());
	} else {
		throw endpoint_error("interface capture method not recognised");
	}

	if (ep.netif()) {
		_sock->bind(ep.netif());
	}

	if (log->enabled(logger::log_notice)) {
		log_message msg(*log, logger::log_notice);
		msg << "capturing on ";
		if (ep.netif()) {
			msg << "interface " << ep.netifname();
		} else {
			msg << "all interfaces";
		}
	}

	if (ep.promiscuous()) {
		if (!ep.netif()) {
			throw endpoint_error("cannot enable promiscuous "
				"mode for all interfaces.");
		}
		_sock->set_promiscuous(ep.netif());

		if (log->enabled(logger::log_notice)) {
			log_message msg(*log, logger::log_notice);
			msg << "enabled promiscuous mode on interface "
				<< ep.netifname();
		}
	}
}

const record& netif_event_reader::read() {
	return _sock->read();
};

void netif_event_reader::attach(const filter& filt) {
	_sock->attach(filt);
}

void netif_event_reader::build_session_start(record_builder& builder) {
	builder.append(std::make_unique<netif_attribute>(
		_ep->netif(), _ep->netifname(), _ep->netif().linktype(),
		_ep->netif().hwaddr()));
}

} /* namespace horace */
