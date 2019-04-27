// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/record.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"
#include "horace/packet_record.h"
#include "horace/unrecognised_record.h"

namespace horace {

record::record(octet_reader& in, size_t length) {
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

void record::append(std::shared_ptr<attribute> attr) {
	_attributes.push_back(attr);
}

size_t record::length() const {
	size_t len = 0;
	for (auto&& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::base128_length(attr->type());
		len += octet_writer::base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

void record::write(std::ostream& out) const {
	out << type_name() << "(";
	for (auto&& attr : _attributes) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

void record::write(octet_writer& out) const {
	out.write_base128(type());
	out.write_base128(length());
	for (auto&& attr : _attributes) {
		attr->write(out);
	}
}

std::unique_ptr<record> record::parse(octet_reader& in,
	int type, size_t length) {

	std::unique_ptr<record> rec = std::make_unique<unrecognised_record>(in, type, length);
	switch (rec->type()) {
	case REC_SESSION_START:
		rec = std::make_unique<session_start_record>(std::move(*rec));
		break;
	case REC_SESSION_END:
		rec = std::make_unique<session_end_record>(std::move(*rec));
		break;
	case REC_PACKET:
		rec = std::make_unique<packet_record>(std::move(*rec));
		break;
	}
	return rec;
};

std::unique_ptr<record> record::parse(octet_reader& in) {
	int type = in.read_base128();
	size_t length = in.read_base128();
	return parse(in, type, length);
}

} /* namespace horace */
