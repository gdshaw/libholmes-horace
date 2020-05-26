// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/address_info.h"

namespace horace {

address_info::address_info(const std::string& hostname,
	const std::string& portname, int socktype, bool server):
	_res(0) {

	struct addrinfo hints = {0};
	hints.ai_family = server ? AF_INET6 : AF_UNSPEC;
	hints.ai_socktype = socktype;
	hints.ai_protocol = 0;
	hints.ai_flags = server ? AI_PASSIVE : AI_ADDRCONFIG;
	const char* chostname = (hostname.empty()) ? 0 : hostname.c_str();
	const char* cportname = (portname.empty()) ? 0 : portname.c_str();
	int errcode = getaddrinfo(chostname, cportname, &hints, &_res);
	if (errcode) {
		throw std::runtime_error(gai_strerror(errcode));
	}
	if (!_res) {
		throw std::runtime_error("socket address not found");
	}
}

address_info::~address_info() {
	freeaddrinfo(_res);
}

socket_descriptor address_info::make_socket() const {
	return socket_descriptor(
		_res->ai_family, _res->ai_socktype,_res->ai_protocol);
}

void address_info::bind(socket_descriptor& fd) const {
	fd.bind(_res->ai_addr, _res->ai_addrlen);
}

void address_info::connect(socket_descriptor& fd) const {
	fd.connect(_res->ai_addr, _res->ai_addrlen);
}

} /* namespace horace */
