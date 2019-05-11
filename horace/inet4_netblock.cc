// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/inet4_netblock.h"

namespace horace {

inet4_netblock::inet4_netblock(const std::string& str) {
	size_t srcidx = 0;
	for (int dstidx = 0; dstidx != 4; ++dstidx) {
		if (srcidx == str.length()) {
			throw std::invalid_argument("invalid IPv4 address");
		}
		if (dstidx != 0) {
			if (str[srcidx] != '.') {
				throw std::invalid_argument("invalid IPv4 address");
			}
			srcidx += 1;
			if (srcidx == str.length()) {
				throw std::invalid_argument("invalid IPv4 address");
			}
		}
		if (!isdigit(str[srcidx])) {
			throw std::invalid_argument("invalid IPv4 address");
		}
		int value = 0;
		while ((srcidx != str.length()) && (isdigit(str[srcidx]))) {
			value *= 10;
			value += str[srcidx] - '0';
			srcidx += 1;
		}
		if (value > 255) {
			throw std::invalid_argument("invalid IPv4 address");
		}
		_prefix[dstidx] = value;
	}
	if (srcidx != str.length()) {
		if (str[srcidx] != '/') {
			throw std::invalid_argument("invalid IPv4 netmask");
		}
		srcidx += 1;
		if (srcidx == str.length()) {
			throw std::invalid_argument("invalid IPv4 netmask");
		}
		if (!isdigit(str[srcidx])) {
			throw std::invalid_argument("invalid IPv4 netmask");
		}
		int value = 0;
		while ((srcidx != str.length()) && (isdigit(str[srcidx]))) {
			value *= 10;
			value += str[srcidx] - '0';
			srcidx += 1;
		}
		if (value > 32) {
			throw std::invalid_argument("invalid IPv4 netmask");
		}
		_prefix_length = value;
	} else {
		_prefix_length = 32;
	}
	if (srcidx != str.length()) {
		throw std::invalid_argument("invalid IPv4 netmask");
	}
}

} /* namespace horace */
