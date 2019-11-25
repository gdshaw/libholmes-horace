// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/session_context.h"
#include "horace/compound_attribute.h"
#include "horace/record_builder.h"

namespace horace {

record_builder::record_builder(int channel):
	record(channel) {}

record_builder::record_builder(session_context& session, octet_reader& in):
	record(in.read_signed_base128()) {

	size_t remaining = in.read_unsigned_base128();
	while (remaining) {
		size_t hdr_len = 0;
		int attr_type = in.read_signed_base128(hdr_len);
		int attr_len = in.read_unsigned_base128(hdr_len);

		std::unique_ptr<attribute> attr =
			attribute::parse(session, attr_type, attr_len, in);
		switch (attr->type()) {
		case attr_type_def:
			session.handle_attr_type_def(
				dynamic_cast<compound_attribute&>(*attr));
			break;
		case attr_channel_def:
			session.handle_channel_def(
				dynamic_cast<compound_attribute&>(*attr));
			break;
		default:
			// no action
			break;
		}
		append(attr);

		size_t length = hdr_len + attr_len;
		if (length > remaining) {
			throw horace_error(
				"attribute extends beyond length of record");
		}
		remaining -= length;
	}
}

record_builder& record_builder::append(std::unique_ptr<attribute>& attr) {
	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

record_builder& record_builder::append(std::unique_ptr<attribute>&& attr) {
	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

record_builder& record_builder::append(const attribute& attr) {
	_attributes.push_back(&attr);
	return *this;
}

void record_builder::reset() {
	_attributes.clear();
}

std::unique_ptr<record> record_builder::build() {
	return std::make_unique<record>(std::move(*this));
}

} /* namespace horace */
