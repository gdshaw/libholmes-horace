// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/sync_record.h"
#include "horace/ack_record.h"

namespace horace {

ack_record::ack_record(record&& rec):
	record(std::move(rec)),
	_timestamp_attr(0),
	_seqnum_attr(0) {

	if (type() != REC_ACK) {
		throw horace_error("incorrect type code for ack record");
	}

	for (auto attr : attributes()) {
		if (std::shared_ptr<absolute_timestamp_attribute> timestamp_attr =
			std::dynamic_pointer_cast<absolute_timestamp_attribute>(attr)) {

			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in ack record");
			}
			_timestamp_attr = timestamp_attr;
		} else if (std::shared_ptr<seqnum_attribute> seqnum_attr =
			std::dynamic_pointer_cast<seqnum_attribute>(attr)) {

			if (_seqnum_attr) {
				throw horace_error(
					"duplicate sequence number attribute in ack record");
			}
			_seqnum_attr = seqnum_attr;
		}
	}

	if (!_timestamp_attr) {
		throw horace_error(
			"missing timestamp attribute in ack record");
	}
	if (!_seqnum_attr) {
		throw horace_error(
			"missing sequence number attribute in ack record");
	}
}

bool ack_record::matches(const sync_record& rec) const {
	return (_timestamp_attr->equals(rec.timestamp()) &&
		(_seqnum_attr->seqnum() == rec.seqnum().seqnum()));
}

} /* namespace horace */
