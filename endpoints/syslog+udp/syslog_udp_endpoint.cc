// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "syslog_udp_endpoint.h"
#include "syslog_udp_event_reader.h"

namespace horace {

syslog_udp_endpoint::syslog_udp_endpoint(const std::string& name):
	endpoint(name) {

	std::string hostportname = this->name().authority().value_or("");
	size_t index = hostportname.rfind(':');
	if (index == std::string::npos) {
		_hostname = hostportname;
		_portname = "syslog";
	} else {
		_hostname = hostportname.substr(0, index);
		_portname = hostportname.substr(
			index + 1, std::string::npos);
	}
}

std::unique_ptr<event_reader> syslog_udp_endpoint::make_event_reader(
	session_builder& session) {

	return std::make_unique<syslog_udp_event_reader>(*this, session);
};

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::syslog_udp_endpoint>(name);
}
