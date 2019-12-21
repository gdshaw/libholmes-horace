// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unrecognised_attribute.h"

namespace horace {

unrecognised_attribute::unrecognised_attribute(int attrid, size_t length,
	octet_reader& in):
	attribute(attrid),
	_length(length) {

	std::unique_ptr<char[]> content = std::make_unique<char[]>(_length);
	in.read(content.get(), _length);
	_content = std::move(content);
}

unrecognised_attribute::unrecognised_attribute(int attrid, size_t length,
	std::unique_ptr<const char[]>& content):
	attribute(attrid),
	_content(std::move(content)),
	_length(length) {}

bool unrecognised_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const unrecognised_attribute* _that =
		dynamic_cast<const unrecognised_attribute*>(&that);
	if (!_that) {
		 return false;
	}
	if (_length != _that->_length) {
		return false;
	}
	if (memcmp(_content.get(), _that->_content.get(), _length)) {
		return false;
	}
	return true;
}

size_t unrecognised_attribute::length() const {
	return _length;
}

std::unique_ptr<attribute> unrecognised_attribute::clone() const {
	std::unique_ptr<char[]> content = std::make_unique<char[]>(_length);
	memcpy(content.get(), _content.get(), _length);
	std::unique_ptr<const char[]> const_content = std::move(content);
	return std::make_unique<unrecognised_attribute>(attrid(), _length, const_content);
}

void unrecognised_attribute::write(std::ostream& out) const {
	out << "attr" << std::dec << attrid() << "(";
	out << std::hex << std::setfill('0');
	for (size_t i = 0; i != _length; ++i) {
		out << std::setw(2) << (_content[i] & 0xff);
	}
	out << std::dec << ")";
}

void unrecognised_attribute::write(octet_writer& out) const {
	out.write_signed_base128(attrid());
	out.write_unsigned_base128(_length);
	out.write(_content.get(), _length);
}

} /* namespace horace */
