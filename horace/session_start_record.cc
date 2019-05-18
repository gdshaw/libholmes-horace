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
		if (std::shared_ptr<source_attribute> source_attr =
			std::dynamic_pointer_cast<source_attribute>(attr)) {

			if (_source_attr) {
				throw horace_error(
					"duplicate source attribute in start of session record");
			}
			_source_attr = source_attr;
		} else if (std::shared_ptr<absolute_timestamp_attribute> timestamp_attr =
			std::dynamic_pointer_cast<absolute_timestamp_attribute>(attr)) {

			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in start of session record");
			}
			_timestamp_attr = timestamp_attr;
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
		struct timespec ts = *_timestamp_attr;
		msg << "start of session (source=" <<
			_source_attr->source_id() << ", ts=" <<
			ts.tv_sec << "." << std::setfill('0') <<
			std::setw(9) << ts.tv_nsec << ")";
	}
}

bool session_start_record::matches(const session_start_record& rec) const {
	return (_source_attr->source_id() == rec._source_attr->source_id()) &&
		(_timestamp_attr->equals(*rec._timestamp_attr));
}

} /* namespace horace */
