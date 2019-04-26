// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/session_end_record.h"

namespace horace {

session_end_record::session_end_record(record&& rec):
	record(std::move(rec)),
	_timestamp_attr(0) {

	for (auto attr : attributes()) {
		if (std::shared_ptr<timestamp_attribute> timestamp_attr =
			std::dynamic_pointer_cast<timestamp_attribute>(attr)) {

			if (_timestamp_attr) {
				throw horace_error(
					"duplicate timestamp attribute in end of session record");
			}
			_timestamp_attr = timestamp_attr;
		}
	}

	if (!_timestamp_attr) {
		throw horace_error(
			"missing timestamp attribute in end of session record");
	}
}

session_end_record::session_end_record(
	std::shared_ptr<timestamp_attribute> timestamp_attr):
	_timestamp_attr(timestamp_attr) {

	append(timestamp_attr);
}

} /* namespace horace */