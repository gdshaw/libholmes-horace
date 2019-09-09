// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/packet_record.h"

namespace horace {

packet_record::packet_record(record&& rec):
	record(std::move(rec)),
	_packet_attr(0),
	_origlen_attr(0),
	_timestamp_attr(0),
	_repeat_attr(0) {

	if (channel_number() != channel_packet) {
		throw horace_error("incorrect channel number for packet record");
	}

	for (auto attr : attributes()) {
		if (attr->type() == attribute::ATTR_PACKET) {
			if (_packet_attr) {
				throw horace_error(
					"duplicate packet attribute in packet record");
			}
			_packet_attr = &dynamic_cast<const binary_ref_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_PACKET_LENGTH) {
			if (_origlen_attr) {
				throw horace_error("duplicate packet "
					"length attribute in packet record");
			}
			_origlen_attr = &dynamic_cast<const unsigned_integer_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_TIMESTAMP) {
			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in packet record");
			}
			_timestamp_attr = &dynamic_cast<const timestamp_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_REPEAT) {
			if (_repeat_attr) {
				throw horace_error(
					"duplicate repeat attribute in packet record");
			}
			_repeat_attr = &dynamic_cast<const unsigned_integer_attribute&>(*attr);
		}
	}
}

void packet_record::log(logger& log) const {
	if (log.enabled(logger::log_debug)) {
		log_message msg(log, logger::log_debug);
		if (_packet_attr) {
			size_t length = _packet_attr->length();
			if (_origlen_attr) {
				length = _origlen_attr->length();
			}
			msg << "packet (length=" << length << ")";
		} else {
			msg << "packet (dropped)";
		}
	}
}

} /* namespace horace */
