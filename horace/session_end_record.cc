// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/session_end_record.h"

namespace horace {

session_end_record::session_end_record(record&& rec):
	record(std::move(rec)),
	_timestamp_attr(0) {

	if (channel_number() != channel_session_end) {
		throw horace_error("incorrect channel number for session end record");
	}

	for (auto attr : attributes()) {
		if (attr->type() == attribute::ATTR_TIMESTAMP) {
			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in end of session record");
			}
			_timestamp_attr = &dynamic_cast<const timestamp_attribute&>(*attr);
		}
	}

	if (!_timestamp_attr) {
		throw horace_error(
			"missing timestamp attribute in end of session record");
	}
}

void session_end_record::log(logger& log) const {
	if (log.enabled(logger::log_notice)) {
		log_message msg(log, logger::log_notice);
		msg << "end of session";
	}
}

} /* namespace horace */
