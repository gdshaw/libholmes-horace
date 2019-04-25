// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/unrecognised_record.h"

namespace horace {

unrecognised_record::unrecognised_record(octet_reader& in,
	int type, size_t length):
	_type(type) {

	while (length) {
		size_t hdr_len = 0;
		int attr_type = in.read_base128(hdr_len);
		int attr_len = in.read_base128(hdr_len);
		std::unique_ptr<attribute> attr =
			attribute::parse(in, attr_type, attr_len);
		_attributes.push_back(std::move(attr));

		size_t full_len = hdr_len + attr_len;
		if (full_len > length) {
			throw horace_error(
				"attribute extends beyond length of record");
		}
		length -= full_len;
	}
}

size_t unrecognised_record::length() const {
	size_t len = 0;
	for (auto&& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::base128_length(attr->type());
		len += octet_writer::base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

void unrecognised_record::write(std::ostream& out) const {
	out << "rec" << std::dec << type() << "(";
	for (auto&& attr : _attributes) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

void unrecognised_record::write(octet_writer& out) const {
	out.write_base128(_type);
	out.write_base128(length());
	for (auto&& attr : _attributes) {
		attr->write(out);
	}
}

} /* namespace horace */
