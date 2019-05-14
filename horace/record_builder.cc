// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"
#include "horace/sync_record.h"
#include "horace/ack_record.h"
#include "horace/packet_record.h"
#include "horace/unrecognised_record.h"
#include "horace/record_builder.h"

namespace horace {

record_builder::record_builder(int type):
	record(type) {}

record_builder::record_builder(octet_reader& in):
	record(in.read_base128()) {

	size_t remaining = in.read_base128();
	while (remaining) {
		size_t hdr_len = 0;
		int attr_type = in.read_base128(hdr_len);
		int attr_len = in.read_base128(hdr_len);
		std::unique_ptr<attribute> attr =
			attribute::parse(in, attr_type, attr_len);
		_attributes.push_back(std::move(attr));

		size_t length = hdr_len + attr_len;
		if (length > remaining) {
			throw horace_error(
				"attribute extends beyond length of record");
		}
		remaining -= length;
	}
}

record_builder& record_builder::append(std::shared_ptr<attribute> attr) {
	_attributes.push_back(attr);
	return *this;
}

void record_builder::reset() {
	_attributes.clear();
}

std::unique_ptr<record> record_builder::build() {
	switch (type()) {
	case REC_SESSION_START:
		return std::make_unique<session_start_record>(std::move(*this));
	case REC_SESSION_END:
		return std::make_unique<session_end_record>(std::move(*this));
	case REC_SYNC:
		return std::make_unique<sync_record>(std::move(*this));
	case REC_ACK:
		return std::make_unique<ack_record>(std::move(*this));
	case REC_PACKET:
		return std::make_unique<packet_record>(std::move(*this));
	}
	return std::make_unique<unrecognised_record>(std::move(*this));
}

} /* namespace horace */
