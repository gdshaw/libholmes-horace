// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/sync_record.h"
#include "horace/ack_record.h"

namespace horace {

ack_record::ack_record(record&& rec):
	record(std::move(rec)),
	_timestamp_attr(0),
	_seqnum_attr(0) {

	if (channel_number() != channel_ack) {
		throw horace_error("incorrect channel number for ack record");
	}

	for (auto attr : attributes()) {
		if (attr->type() == attribute::ATTR_TIMESTAMP) {
			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in ack record");
			}
			_timestamp_attr = &dynamic_cast<const timestamp_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_SEQNUM) {
			if (_seqnum_attr) {
				throw horace_error(
					"duplicate sequence number attribute in ack record");
			}
			_seqnum_attr = &dynamic_cast<const unsigned_integer_attribute&>(*attr);
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

void ack_record::log(logger& log) const {
	if (log.enabled(logger::log_info)) {
		log_message msg(log, logger::log_info);
		struct timespec ts = _timestamp_attr->content();
		msg << "sync request (ts=" <<
			ts.tv_sec << "." << std::setfill('0') <<
			std::setw(9) << ts.tv_nsec <<
			" seqnum=" << _seqnum_attr->content() << ")";
	}
}

bool ack_record::matches(const sync_record& rec) const {
	return (_timestamp_attr->content().tv_sec == rec.timestamp().content().tv_sec) &&
		(_timestamp_attr->content().tv_nsec == rec.timestamp().content().tv_nsec) &&
		(_seqnum_attr->content() == rec.seqnum().content());
}

} /* namespace horace */
