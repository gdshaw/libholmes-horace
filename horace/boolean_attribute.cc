// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_base128_integer.h"
#include "horace/signed_base128_integer.h"
#include "horace/boolean_attribute.h"

namespace horace {

boolean_attribute::boolean_attribute(int attrid,
	bool content):
	attribute(attrid),
	_content(content) {}

boolean_attribute::boolean_attribute(int attrid,
	size_t length, octet_reader& in):
	attribute(attrid) {

	if (length != 1) {
		throw horace_error(
			"invalid length for boolean attribute");
	}
	char octet = in.read();
	if ((octet != 0) && (octet != 1)) {
		throw horace_error(
			"invalid content for boolean attribute");
	}
	_content = octet;
}

bool boolean_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const boolean_attribute* _that =
		dynamic_cast<const boolean_attribute*>(&that);
	if (!_that) {
		return false;
	}
	return (_content == _that->_content);
}

size_t boolean_attribute::length() const {
	return 1;
}

std::unique_ptr<attribute> boolean_attribute::clone() const {
	return std::make_unique<boolean_attribute>(
		attrid(), _content);
}

void boolean_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" << std::dec << _content << ")";
}

void boolean_attribute::write(octet_writer& out) const {
	int len = length();
	signed_base128_integer(attrid()).write(out);
	unsigned_base128_integer(len).write(out);
	out.write(_content);
}

} /* namespace horace */
