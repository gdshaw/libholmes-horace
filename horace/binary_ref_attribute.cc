// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/binary_ref_attribute.h"

namespace horace {

binary_ref_attribute::binary_ref_attribute(int type, size_t length, const void* content):
	attribute(type),
	_content(static_cast<const char*>(content)),
	_length(length) {}

void binary_ref_attribute::write(std::ostream& out) const {
	out << "attr" << type() << "(" << std::hex << std::setfill('0');
	for (size_t i = 0; i != _length; ++i) {
		out << std::setw(2) << (_content[i] & 0xff);
	}
	out << std::dec << ")";
}

void binary_ref_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(_length);
	out.write(_content, _length);
}

std::unique_ptr<attribute> binary_ref_attribute::clone() const {
	return std::make_unique<binary_ref_attribute>(
		type(), _length, _content);
}

} /* namespace horace */
