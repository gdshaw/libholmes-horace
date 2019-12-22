// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/query_string.h"

#include "unix_endpoint.h"
#include "unix_event_reader.h"

namespace horace {

unix_endpoint::unix_endpoint(const std::string& name):
	endpoint(name),
	_snaplen(0x40000) {

	std::optional<std::string> query = this->name().query();
	if (query) {
		query_string params(*query);
		_snaplen = params.find<long>("snaplen").value_or(_snaplen);
	}
}

std::unique_ptr<event_reader> unix_endpoint::make_event_reader(
	session_builder& session) {

	return std::make_unique<unix_event_reader>(*this, session);
};

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::unix_endpoint>(name);
}
