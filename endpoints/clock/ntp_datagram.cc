// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "ntp_error.h"
#include "ntp_datagram.h"

namespace horace {

ntp_datagram::ntp_datagram(std::basic_string<unsigned char> content):
	_content(content) {

	if (_content.length() < 12) {
		throw ntp_error("datagram too short");
	}
}

ntp_datagram::ntp_datagram(unsigned int opcode, uint16_t seqnum,
	uint16_t associd):
	_content(12, 0) {

	_content[0] = 0x16;
	_content[1] = opcode;
	_content[2] = (seqnum >> 8) & 0xff;
	_content[3] = (seqnum >> 0) & 0xff;
	_content[6] = (associd >> 8) & 0xff;
	_content[7] = (associd >> 0) & 0xff;
}

bool ntp_datagram::responds_to(const ntp_datagram& request) {
	if (!is_response()) {
		return false;
	}
	if (opcode() != request.opcode()) {
		return false;
	}
	if (seqnum() != request.seqnum()) {
		return false;
	}
	if (associd() != request.associd()) {
		return false;
	}
	return true;
}

} /* namespace horace */
