// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/repeat_attribute.h"

namespace horace {

repeat_attribute::repeat_attribute(uint64_t count):
	attribute(ATTR_REPEAT),
	_count(count) {
}

repeat_attribute::repeat_attribute(octet_reader& in, size_t length):
	attribute(ATTR_REPEAT),
	_count(in.read_unsigned(length)) {}

size_t repeat_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _count >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void repeat_attribute::write(std::ostream& out) const {
	out << "count(" << std::dec << _count << ")";
}

void repeat_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(type());
	out.write_unsigned_base128(len);
	out.write_unsigned(_count, len);
}

} /* namespace horace */
