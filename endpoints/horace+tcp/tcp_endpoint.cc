// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/query_string.h"

#include "tcp_session_listener.h"
#include "tcp_session_writer.h"
#include "tcp_endpoint.h"

namespace horace {

tcp_endpoint::tcp_endpoint(const std::string& name):
	endpoint(name),
	_retry(30),
	_diode(false) {

	std::string hostportname = this->name().authority().value_or("");
	size_t index = hostportname.rfind(':');
	if (index == std::string::npos) {
		_hostname = hostportname;
		_portname = "40960";
	} else {
		_hostname = hostportname.substr(0, index);
		_portname = hostportname.substr(
			index + 1, std::string::npos);
	}

	if (std::optional<std::string> query = this->name().query()) {
		query_string params(*query);
		_retry = params.find<long>("retry").value_or(_retry);
		_diode = params.find<bool>("diode").value_or(_diode);
	}
}

std::unique_ptr<session_listener> tcp_endpoint::make_session_listener() {
	return std::make_unique<tcp_session_listener>(*this);
}

std::unique_ptr<session_writer> tcp_endpoint::make_session_writer(
	const std::string& srcid) {

	return std::make_unique<tcp_session_writer>(*this, srcid);
}

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::tcp_endpoint>(name);
}
