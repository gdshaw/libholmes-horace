// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "string_attribute.h"

namespace horace {

string_attribute::string_attribute(int type, const std::string& content):
	attribute(type),
	_content(content) {}

string_attribute::string_attribute(int type, size_t length, octet_reader& in):
	attribute(type),
	_content(in.read_string(length)) {}

size_t string_attribute::length() const {
	return _content.length();
}

void string_attribute::write(std::ostream& out) const {
	out << "attr" << type() << "(" << _content << ")";
}

void string_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(length());
	out.write_string(_content);
}

} /* namespace horace */
