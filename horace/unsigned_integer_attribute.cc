// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_base128_integer.h"
#include "horace/signed_base128_integer.h"
#include "horace/unsigned_integer_attribute.h"

namespace horace {

unsigned_integer_attribute::unsigned_integer_attribute(int attrid,
	uint64_t content):
	attribute(attrid),
	_content(content) {}

unsigned_integer_attribute::unsigned_integer_attribute(int attrid,
	size_t length, octet_reader& in):
	attribute(attrid) {

	if (length > 8) {
		throw horace_error(
			"invalid length for unsigned integer attribute");
	}
	_content = in.read_unsigned(length);
}

bool unsigned_integer_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const unsigned_integer_attribute* _that =
		dynamic_cast<const unsigned_integer_attribute*>(&that);
	if (!_that) {
		return false;
	}
	return (_content == _that->_content);
}

size_t unsigned_integer_attribute::length() const {
	size_t len = 0;
	uint64_t rem = _content;
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
	signed_base128_integer(attrid()).write(out);
	unsigned_base128_integer(len).write(out);
	out.write_unsigned(_content, len);
}

} /* namespace horace */
