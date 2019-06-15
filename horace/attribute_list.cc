// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/attribute_list.h"

namespace horace {

attribute_list::~attribute_list() {
	for (const attribute* attr : _owned_attributes) {
		delete attr;
	}
}

attribute_list::attribute_list(attribute_list&& that) {
	_attributes.swap(that._attributes);
	_owned_attributes.swap(that._owned_attributes);
}

attribute_list::attribute_list(octet_reader& in, size_t length) {
	size_t remaining = length;
	while (remaining) {
		size_t hdr_len = 0;
		int attr_type = in.read_base128(hdr_len);
		int attr_len = in.read_base128(hdr_len);
		std::unique_ptr<attribute> attr =
			attribute::parse(in, attr_type, attr_len);
		append(attr);

		size_t length = hdr_len + attr_len;
		if (length > remaining) {
			throw horace_error(
				"attribute extends beyond length of record");
		}
		remaining -= length;
	}
}

size_t attribute_list::length() const {
	size_t len = 0;
	for (auto&& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::base128_length(attr->type());
		len += octet_writer::base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

attribute_list& attribute_list::append(
	std::unique_ptr<attribute>& attr) {

	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

attribute_list& attribute_list::append(
	std::unique_ptr<attribute>&& attr) {

	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

attribute_list& attribute_list::append(
	const attribute& attr) {

	_attributes.push_back(&attr);
	return *this;
}

void attribute_list::write(octet_writer& out) const {
	for (auto&& attr : _attributes) {
		attr->write(out);
	}
}

std::ostream& operator<<(std::ostream& out, const attribute_list& rec) {
	out << "(";
	for (auto&& attr : rec.attributes()) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

} /* namespace horace */
