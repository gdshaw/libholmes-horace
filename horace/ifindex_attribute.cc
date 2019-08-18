// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/ifindex_attribute.h"

namespace horace {

ifindex_attribute::ifindex_attribute(uint64_t ifindex):
	_ifindex(ifindex) {
}

ifindex_attribute::ifindex_attribute(octet_reader& in, size_t length):
	_ifindex(in.read_unsigned(length)) {}

size_t ifindex_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _ifindex >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void ifindex_attribute::write(std::ostream& out) const {
	out << "ifindex(" << std::dec << _ifindex << ")";
}

void ifindex_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(type());
	out.write_unsigned_base128(len);
	out.write_unsigned(_ifindex, len);
}

} /* namespace horace */
