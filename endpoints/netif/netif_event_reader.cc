// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/endpoint_error.h"
#include "horace/string_attribute.h"
#include "horace/binary_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/session_builder.h"
#include "horace/packet_record_builder.h"

#include "packet_socket.h"
#include "ring_buffer_v1.h"
#include "ring_buffer_v2.h"
#include "ring_buffer_v3.h"
#include "netif_event_reader.h"
#include "netif_endpoint.h"

namespace horace {

class record;

netif_event_reader::netif_event_reader(const netif_endpoint& ep,
	session_builder& session):
	_ep(&ep) {

	attribute_list attrs;
	attrs.append(std::make_unique<unsigned_integer_attribute>(
		session.define_attribute("ifindex", attrfmt_unsigned_integer),
		_ep->netif().ifindex()));
	attrs.append(std::make_unique<string_attribute>(
		session.define_attribute("ifname", attrfmt_string),
		_ep->netifname()));
	attrs.append(std::make_unique<unsigned_integer_attribute>(
		session.define_attribute("linktype", attrfmt_unsigned_integer),
		_ep->netif().linktype()));
	attrs.append(std::make_unique<binary_attribute>(
		session.define_attribute("hwaddr", attrfmt_binary),
		_ep->netif().hwaddr().length(), _ep->netif().hwaddr().data()));
	_channel = session.define_channel("packets", std::move(attrs));

	_builder = std::make_unique<packet_record_builder>(session, _channel);
	_sock = ep.make_basic_packet_socket(*_builder);

	if (!ep.netif().isany()) {
		_sock->bind(ep.netif());
	}

	if (log->enabled(logger::log_notice)) {
		log_message msg(*log, logger::log_notice);
		msg << "capturing on ";
		if (ep.netif().isany()) {
			msg << "all interfaces";
		} else {
			msg << "interface " << ep.netifname();
		}
		msg << " with " << _sock->method() << " socket";
	}

	if (ep.promiscuous()) {
		if (ep.netif().isany()) {
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

} /* namespace horace */
