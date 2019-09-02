// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/session_start_record.h"

namespace horace {

session_start_record::session_start_record(record&& rec):
	record(std::move(rec)),
	_source_attr(0),
	_timestamp_attr(0) {

	if (type() != REC_SESSION_START) {
		throw horace_error("incorrect type code for session start record");
	}

	for (auto attr : attributes()) {
		if (attr->type() == attribute::ATTR_SOURCE) {
			if (_source_attr) {
				throw horace_error(
					"duplicate source attribute in start of session record");
			}
			_source_attr = &dynamic_cast<const string_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_TIMESTAMP) {
			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in start of session record");
			}
			_timestamp_attr = dynamic_cast<const timestamp_attribute*>(attr);
		} else if (const netif_attribute* netif_attr =
			dynamic_cast<const netif_attribute*>(attr)) {

			_netif_attrs.push_back(netif_attr);
		}
	}

	if (!_source_attr) {
		throw horace_error(
			"missing source attribute in start of session record");
	}
	if (!_timestamp_attr) {
		throw horace_error(
			"missing timestamp attribute in start of session record");
	}
}

void session_start_record::log(logger& log) const {
	if (log.enabled(logger::log_notice)) {
		log_message msg(log, logger::log_notice);
		struct timespec ts = _timestamp_attr->content();
		msg << "start of session (source=" <<
			_source_attr->content() << ", ts=" <<
			ts.tv_sec << "." << std::setfill('0') <<
			std::setw(9) << ts.tv_nsec << ")";
	}
}

bool session_start_record::matches(const session_start_record& rec) const {
	return (_source_attr->content() == rec._source_attr->content()) &&
		(_timestamp_attr->content().tv_sec == rec._timestamp_attr->content().tv_sec) &&
		(_timestamp_attr->content().tv_nsec == rec._timestamp_attr->content().tv_nsec);
}

} /* namespace horace */
