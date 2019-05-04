// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/packet_length_attribute.h"

namespace horace {

packet_length_attribute::packet_length_attribute(octet_reader& in,
	size_t length):
	_origlen(in.read_unsigned(length)) {}

packet_length_attribute::packet_length_attribute(uint64_t origlen):
	_origlen(origlen) {
}

size_t packet_length_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _origlen >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void packet_length_attribute::write(std::ostream& out) const {
	out << "pktlen(" << std::dec << _origlen << ")";
}

void packet_length_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_base128(type());
	out.write_base128(len);
	out.write_unsigned(_origlen, len);
}

} /* namespace horace */
