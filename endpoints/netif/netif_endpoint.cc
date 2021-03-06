// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/endpoint_error.h"
#include "horace/query_string.h"
#include "horace/packet_record_builder.h"

#include "basic_packet_socket.h"
#include "packet_socket.h"
#include "ring_buffer_v1.h"
#include "ring_buffer_v2.h"
#include "ring_buffer_v3.h"
#include "netif_endpoint.h"
#include "netif_event_reader.h"

namespace horace {

netif_endpoint::netif_endpoint(const std::string& name):
	endpoint(name),
	_snaplen(0x40000),
	_capacity(0x1000000),
	_promiscuous(false) {

	std::string ifname = netifname();
	if (!ifname.empty()) {
		_if = interface(ifname);
	}

	std::optional<std::string> query = this->name().query();
	if (query) {
		query_string params(*query);
		_method = params.find<std::string>("capture").value_or(_method);
		_snaplen = params.find<long>("snaplen").value_or(_snaplen);
		_capacity = params.find<long long>("capacity").value_or(_capacity);
		_promiscuous = params.find<bool>("promiscuous").
			value_or(_promiscuous);
	}
}

std::unique_ptr<event_reader> netif_endpoint::make_event_reader(
	session_builder& session) {

	return std::make_unique<netif_event_reader>(*this, session);
};

std::unique_ptr<basic_packet_socket> netif_endpoint::make_basic_packet_socket(
	packet_record_builder& builder) const {

	if (_method.empty()) {
		try {
			return std::make_unique<ring_buffer_v3>(builder, snaplen(), capacity());
		} catch (std::exception& ex) {}
		try {
			return std::make_unique<ring_buffer_v2>(builder, snaplen(), capacity());
		} catch (std::exception& ex) {}
		try {
			return std::make_unique<ring_buffer_v1>(builder, snaplen(), capacity());
		} catch (std::exception& ex) {}
		return std::make_unique<packet_socket>(builder, snaplen(), capacity());
	} else if (_method == "ringv3") {
		return std::make_unique<ring_buffer_v3>(builder, snaplen(), capacity());
	} else if (_method == "ringv2") {
		return std::make_unique<ring_buffer_v2>(builder, snaplen(), capacity());
	} else if (_method == "ringv1") {
		return std::make_unique<ring_buffer_v1>(builder, snaplen(), capacity());
	} else if (_method == "packet") {
		return std::make_unique<packet_socket>(builder, snaplen(), capacity());
	} else {
		throw endpoint_error("unrecognised capture _method");
	}
}

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::netif_endpoint>(name);
}
