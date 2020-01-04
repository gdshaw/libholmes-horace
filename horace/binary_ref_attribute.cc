// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/octet_writer.h"
#include "horace/unsigned_base128_integer.h"
#include "horace/signed_base128_integer.h"
#include "horace/binary_ref_attribute.h"

namespace horace {

binary_ref_attribute::binary_ref_attribute(int attrid, size_t length, const void* content):
	attribute(attrid),
	_content(static_cast<const char*>(content)),
	_length(length) {}

bool binary_ref_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const binary_ref_attribute* _that =
		dynamic_cast<const binary_ref_attribute*>(&that);
	if (!_that) {
		return false;
	}
	if (_length != _that->_length) {
		return false;
	}
	if (memcmp(_content, _that->_content, _length)) {
		return false;
	}
	return true;
}

size_t binary_ref_attribute::length() const {
	return _length;
}

std::unique_ptr<attribute> binary_ref_attribute::clone() const {
	return std::make_unique<binary_ref_attribute>(
		attrid(), _length, _content);
}

void binary_ref_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" << std::hex << std::setfill('0');
	for (size_t i = 0; i != _length; ++i) {
		out << std::setw(2) << (_content[i] & 0xff);
	}
	out << std::dec << ")";
}

void binary_ref_attribute::write(octet_writer& out) const {
	signed_base128_integer(attrid()).write(out);
	unsigned_base128_integer(length()).write(out);
	out.write(_content, _length);
}

} /* namespace horace */
