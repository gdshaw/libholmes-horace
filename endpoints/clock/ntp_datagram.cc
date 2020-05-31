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
	uint16_t assoc_id):
	_content(12, 0) {

	_content[0] = 0x16;
	_content[1] = opcode;
	_content[2] = (seqnum >> 8) & 0xff;
	_content[3] = (seqnum >> 0) & 0xff;
	_content[6] = (assoc_id >> 8) & 0xff;
	_content[7] = (assoc_id >> 0) & 0xff;
}

bool ntp_datagram::responds_to(const ntp_datagram& request) {
	if (!is_response()) {
		throw ntp_error("datagram is not a response");
	}
	if (opcode() != request.opcode()) {
		throw ntp_error("opcodes do not match");
	}
	if (seqnum() != request.seqnum()) {
		throw ntp_error("sequence numbers do not match");
	}
	if (assoc_id() != request.assoc_id()) {
		throw ntp_error("association IDs do not match");
	}
}

} /* namespace horace */
