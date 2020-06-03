// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "ntp_error.h"
#include "ntp_response.h"

namespace horace {

void ntp_response::add(std::unique_ptr<ntp_datagram>& fragment) {
	_fragments[fragment->offset()] = std::move(fragment);
}

bool ntp_response::complete() const {
	size_t offset = 0;
	auto f = _fragments.find(offset);
	while (f != _fragments.end()) {
		if (f->second->more()) {
			offset += f->second->count();
			f = _fragments.find(offset);
		} else {
			return true;
		}
	}
	return false;
}

std::basic_string<unsigned char> ntp_response::payload() const {
	std::basic_string<unsigned char> payload;
	auto f = _fragments.find(payload.length());
	while (f != _fragments.end()) {
		payload += f->second->payload();
		if (f->second->more()) {
			f = _fragments.find(payload.length());
		} else {
			break;
		}
	}
	if (f == _fragments.end()) {
		throw ntp_error("incomplete NTP response");
	}
	return payload;
}

std::string ntp_response::text_payload() const {
	std::string payload;
	auto f = _fragments.find(payload.length());
	while (f != _fragments.end()) {
		auto fragment_payload = f->second->payload();
		auto fragment_text = reinterpret_cast<const char*>(
			fragment_payload.data());
		std::string fragment_string(fragment_text,
			fragment_payload.length());
		payload += fragment_string;

		if (f->second->more()) {
			f = _fragments.find(payload.length());
		} else {
			break;
		}
	}
	if (f == _fragments.end()) {
		throw ntp_error("incomplete NTP response");
	}
	return payload;
}

uint16_t ntp_response::associd() const {
	auto f = _fragments.find(0);
	if (f == _fragments.end()) {
		throw ntp_error("incomplete NTP response");
	}
	return f->second->associd();
}

uint16_t ntp_response::status() const {
	auto f = _fragments.find(0);
	if (f == _fragments.end()) {
		throw ntp_error("incomplete NTP response");
	}
	return f->second->status();
}

} /* namespace horace */
