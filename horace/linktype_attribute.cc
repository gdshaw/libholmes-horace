// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/linktype_attribute.h"

namespace horace {

linktype_attribute::linktype_attribute(uint64_t linktype):
	_linktype(linktype) {
}

linktype_attribute::linktype_attribute(octet_reader& in, size_t length):
	_linktype(in.read_unsigned(length)) {}

size_t linktype_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _linktype >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void linktype_attribute::write(std::ostream& out) const {
	out << "linktype(" << std::dec << _linktype << ")";
}

void linktype_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_base128(type());
	out.write_base128(len);
	out.write_unsigned(_linktype, len);
}

} /* namespace horace */
