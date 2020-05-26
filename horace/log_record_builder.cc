// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/session_builder.h"
#include "horace/log_record_builder.h"

namespace horace {

log_record_builder::log_record_builder(session_builder& session, int channel):
	_channel(channel),
	_ts_attr(attrid_ts, 0, 0),
	_severity_attr(session.define_attribute("syslog_severity", attrfmt_unsigned_integer), 0),
	_facility_attr(session.define_attribute("syslog_facility", attrfmt_unsigned_integer), 0),
	_message_attr(session.define_attribute("message", attrfmt_string), "") {}

void log_record_builder::add_severity(unsigned int severity) {
	_attrs.append(_severity_attr = unsigned_integer_attribute(_severity_attr.attrid(), severity));
}

void log_record_builder::add_facility(unsigned int facility) {
	_attrs.append(_facility_attr = unsigned_integer_attribute(_facility_attr.attrid(), facility));
}

void log_record_builder::add_message(const std::string& message) {
	_attrs.append(_message_attr = string_attribute(_message_attr.attrid(), message));
}

const record& log_record_builder::build() {
	_attrs.append(_ts_attr = timestamp_attribute(_ts_attr.attrid()));
	_built = record(_channel, std::move(_attrs));
	return _built;
}

} /* namespace horace */
