// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "ntp_error.h"
#include "ntp_response.h"
#include "ntp_peer.h"
#include "ntp_assoc_attr_builder.h"

namespace horace {

ntp_peer::ntp_peer(const ntp_response& response):
	_associd(response.associd()),
	_status(response.status()) {

	std::string payload = response.text_payload();
	size_t i = 0;
	size_t j = payload.find(',', i);
	while (j != payload.npos) {
		while ((i != payload.length()) && isspace(payload[i])) {
			i += 1;
		}
		std::string varstr = payload.substr(i, j - i);
		size_t k = varstr.find('=');
		if (k != varstr.npos) {
			std::string name = varstr.substr(0, k);
			std::string value = varstr.substr(k + 1);
			_variables[name] = value;
		}
		i = j + 1;
		j = payload.find(',', i);
	}
}

bool ntp_peer::contains(const std::string& varname) {
	return _variables.find(varname) != _variables.end();
}

std::string ntp_peer::get_string(const std::string& varname) {
	auto f = _variables.find(varname);
	if (f != _variables.end()) {
		return f->second;
	}
	return std::string();
}

std::string ntp_peer::get_quoted_string(const std::string& varname) {
	auto f = _variables.find(varname);
	if (f != _variables.end()) {
		std::string quoted = f->second;
		if ((quoted.length() < 2) || (quoted[0] != '"') ||
			(quoted[quoted.length() - 1] != '"')) {

			throw ntp_error("quoted string expected");
		}
		return quoted.substr(1, quoted.length() - 2);
	}
	return std::string();
}

int64_t ntp_peer::get_integer(const std::string& varname) {
	auto f = _variables.find(varname);
	if (f != _variables.end()) {
		std::string str = f->second;
		int64_t value = 0;
		bool negative = false;
		size_t i = 0;

		if ((i != str.length()) && (str[i] == '-')) {
			i += 1;
			negative = true;
		}

		while ((i != str.length()) && isdigit(str[i])) {
			value *= 10;
			value += str[i++] - '0';
		}

		if (negative) {
			value = -value;
		}
		return value;
	}
	return 0;
}

int64_t ntp_peer::get_fixed(const std::string& varname) {
	auto f = _variables.find(varname);
	if (f != _variables.end()) {
		std::string str = f->second;
		int64_t value = 0;
		bool negative = false;
		size_t i = 0;

		if ((i != str.length()) && (str[i] == '-')) {
			i += 1;
			negative = true;
		}

		while ((i != str.length()) && isdigit(str[i])) {
			value *= 10;
			value += str[i++] - '0';
		}

		unsigned int shift = 6;
		if ((i != str.length()) && (str[i] == '.')) {
			i += 1;
			while ((i != str.length()) && isdigit(str[i]) && (shift != 0)) {
				value *= 10;
				value += str[i++] - '0';
				shift -= 1;
			}
		}

		while (shift != 0) {
			value *= 10;
			shift -= 1;
		}
		if (negative) {
			value = -value;
		}
		return value;
	}
	return 0;
}

void ntp_peer::build(ntp_assoc_attr_builder& builder) {
	// Support both the name used in RFC 5905 (srcaddr)
	// and the name used by ntpd (srcadr).
	builder.add_associd(_associd);
	builder.add_status(_status);
	if (contains("srchost")) {
		builder.add_srchost(get_quoted_string("srchost"));
	}
	if (contains("srcaddr")) {
		builder.add_srcaddr(get_string("srcaddr"));
	} else if (contains("srcadr")) {
		builder.add_srcaddr(get_string("srcadr"));
	}
	if (contains("srcport")) {
		uint16_t srcport = get_integer("srcport");
		if (srcport != 123) {
			builder.add_srcport(srcport);
		}
	}
	if (contains("stratum")) {
		builder.add_stratum(get_integer("stratum"));
	}
	if (contains("delay")) {
		builder.add_delay(get_fixed("delay"));
	}
	if (contains("offset")) {
		builder.add_offset(get_fixed("offset"));
	}
	if (contains("jitter")) {
		builder.add_jitter(get_fixed("jitter"));
	}
}

} /* namespace horace */
