// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/query_string.h"

#include "clock_endpoint.h"
#include "clock_event_reader.h"

namespace horace {

clock_endpoint::clock_endpoint(const std::string& name):
	endpoint(name) {}

std::unique_ptr<event_reader> clock_endpoint::make_event_reader(
	session_builder& session) {

	return std::make_unique<clock_event_reader>(*this, session);
};

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::clock_endpoint>(name);
}
