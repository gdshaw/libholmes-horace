// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/seqnum_attribute.h"

namespace horace {

seqnum_attribute::seqnum_attribute(uint64_t seqnum):
	_seqnum(seqnum) {
}

seqnum_attribute::seqnum_attribute(octet_reader& in, size_t length):
	_seqnum(in.read_unsigned(length)) {}

size_t seqnum_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _seqnum >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void seqnum_attribute::write(std::ostream& out) const {
	out << "seqnum(" << std::dec << _seqnum << ")";
}

void seqnum_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_base128(type());
	out.write_base128(len);
	out.write_unsigned(_seqnum, len);
}

} /* namespace horace */
