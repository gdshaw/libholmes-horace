// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_integer_attribute.h"

namespace horace {

unsigned_integer_attribute::unsigned_integer_attribute(int attrid,
	uint64_t content):
	attribute(attrid),
	_content(content) {}

unsigned_integer_attribute::unsigned_integer_attribute(int attrid,
	size_t length, octet_reader& in):
	attribute(attrid) {

	if ((length < 1) || (length > 8)) {
		throw horace_error(
			"invalid length for unsigned integer attribute");
	}
	_content = in.read_unsigned(length);
}

size_t unsigned_integer_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _content >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

std::unique_ptr<attribute> unsigned_integer_attribute::clone() const {
	return std::make_unique<unsigned_integer_attribute>(
		attrid(), _content);
}

void unsigned_integer_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" << std::dec << _content << ")";
}

void unsigned_integer_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(attrid());
	out.write_unsigned_base128(len);
	out.write_unsigned(_content, len);
}

} /* namespace horace */
