// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_base128_integer.h"
#include "horace/signed_base128_integer.h"
#include "horace/string_attribute.h"

namespace horace {

string_attribute::string_attribute(int attrid, const std::string& content):
	attribute(attrid),
	_content(content) {}

string_attribute::string_attribute(int attrid, size_t length, octet_reader& in):
	attribute(attrid),
	_content(in.read_string(length)) {}

bool string_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const string_attribute* _that =
		dynamic_cast<const string_attribute*>(&that);
	if (!_that) {
		return false;
	}
	return (_content == _that->_content);
}

size_t string_attribute::length() const {
	return _content.length();
}

std::unique_ptr<attribute> string_attribute::clone() const {
	return std::make_unique<string_attribute>(attrid(), _content);
}

void string_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" << _content << ")";
}

void string_attribute::write(octet_writer& out) const {
	signed_base128_integer(attrid()).write(out);
	unsigned_base128_integer(length()).write(out);
	out.write_string(_content);
}

} /* namespace horace */
