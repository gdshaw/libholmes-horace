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
#include "horace/signed_integer_attribute.h"

namespace horace {

signed_integer_attribute::signed_integer_attribute(int attrid,
	int64_t content):
	attribute(attrid),
	_content(content) {}

signed_integer_attribute::signed_integer_attribute(int attrid,
	size_t length, octet_reader& in):
	attribute(attrid) {

	if ((length < 1) || (length > 8)) {
		throw horace_error(
			"invalid length for signed integer attribute");
	}
	_content = in.read_signed(length);
}

bool signed_integer_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const signed_integer_attribute* _that =
		dynamic_cast<const signed_integer_attribute*>(&that);
	if (!_that) {
		return false;
	}
	return (_content == _that->_content);
}

size_t signed_integer_attribute::length() const {
	int64_t term = (_content >= 0) ? 0 : -1;
	unsigned int size = 7;
	size_t count = 1;
	while ((_content >> size) != term) {
		size += 8;
		count += 1;
	}
	return count;
}

std::unique_ptr<attribute> signed_integer_attribute::clone() const {
	return std::make_unique<signed_integer_attribute>(
		attrid(), _content);
}

void signed_integer_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" << std::dec << _content << ")";
}

void signed_integer_attribute::write(octet_writer& out) const {
	int len = length();
	signed_base128_integer(attrid()).write(out);
	unsigned_base128_integer(len).write(out);
	out.write_signed(_content, len);
}

} /* namespace horace */
