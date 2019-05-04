// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/query_string.h"

#include "netif_endpoint.h"
#include "netif_event_reader.h"

namespace horace {

netif_endpoint::netif_endpoint(const std::string& name):
	endpoint(name) {

	std::string path = this->name().path();
	std::optional<std::string> query = this->name().query();

	if (!path.empty()) {
		_if = interface(path);
	}

	if (query) {
		query_string params(*query);
		_snaplen = params.find<long>("snaplen").value_or(0x40000);
	}
}

std::unique_ptr<event_reader> netif_endpoint::make_event_reader() {
	return std::make_unique<netif_event_reader>(*this);
};

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::netif_endpoint>(name);
}
