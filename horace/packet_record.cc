// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/packet_record.h"

namespace horace {

packet_record::packet_record(record&& rec):
	record(std::move(rec)),
	_packet_attr(0),
	_timestamp_attr(0) {

	if (type() != REC_PACKET) {
		throw horace_error("incorrect type code for packet record");
	}

	for (auto attr : attributes()) {
		if (std::shared_ptr<packet_attribute> packet_attr =
			std::dynamic_pointer_cast<packet_attribute>(attr)) {

			if (_packet_attr) {
				throw horace_error(
					"duplicate packet attribute in packet record");
			}
			_packet_attr = packet_attr;
		} else if (std::shared_ptr<packet_length_attribute> origlen_attr =
			std::dynamic_pointer_cast<packet_length_attribute>(attr)) {

			if (_origlen_attr) {
				throw horace_error("duplicate packet "
					"length attribute in packet record");
			}
			_origlen_attr = origlen_attr;
		} else if (std::shared_ptr<timestamp_attribute> timestamp_attr =
			std::dynamic_pointer_cast<timestamp_attribute>(attr)) {

			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in packet record");
			}
			_timestamp_attr = timestamp_attr;
		}
	}
}

} /* namespace horace */
