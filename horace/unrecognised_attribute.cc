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

unrecognised_attribute::unrecognised_attribute(int type, size_t length,
	octet_reader& in):
	attribute(type),
	_length(length) {

	std::unique_ptr<char[]> content = std::make_unique<char[]>(_length);
	in.read(content.get(), _length);
	_content = std::move(content);
}

unrecognised_attribute::unrecognised_attribute(int type,
	std::unique_ptr<const char[]>& content, size_t length):
	attribute(type),
	_content(move(content)),
	_length(length) {}

std::unique_ptr<attribute> unrecognised_attribute::clone() const {
	std::unique_ptr<char[]> content = std::make_unique<char[]>(_length);
	memcpy(content.get(), _content.get(), _length);
	std::unique_ptr<const char[]> const_content = std::move(content);
	return std::make_unique<unrecognised_attribute>(type(), const_content, _length);
}

void unrecognised_attribute::write(std::ostream& out) const {
	out << "attr" << std::dec << type() << "(";
	out << std::hex << std::setfill('0');
	for (size_t i = 0; i != _length; ++i) {
		out << std::setw(2) << (_content[i] & 0xff);
	}
	out << std::dec << ")";
}

void unrecognised_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(_length);
	out.write(_content.get(), _length);
}

} /* namespace horace */
