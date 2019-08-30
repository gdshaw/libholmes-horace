// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "source_attribute.h"

namespace horace {

source_attribute::source_attribute(const std::string& source_id):
	attribute(ATTR_SOURCE),
	_source_id(source_id) {}

source_attribute::source_attribute(octet_reader& in, size_t length):
	attribute(ATTR_SOURCE),
	_source_id(in.read_string(length)) {}

size_t source_attribute::length() const {
	return _source_id.length();
}

void source_attribute::write(std::ostream& out) const {
	out << "source(" << _source_id << ")";
}

void source_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(length());
	out.write_string(_source_id);
}

} /* namespace horace */
