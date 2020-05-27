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
	_version_attr(session.define_attribute("syslog_version", attrfmt_unsigned_integer), 0),
	_timestamp_attr(session.define_attribute("syslog_timestamp", attrfmt_string), ""),
	_hostname_attr(session.define_attribute("syslog_hostname", attrfmt_string), ""),
	_appname_attr(session.define_attribute("syslog_appname", attrfmt_string), ""),
	_procid_attr(session.define_attribute("syslog_procid", attrfmt_string), ""),
	_msgid_attr(session.define_attribute("syslog_msgid", attrfmt_string), ""),
	_message_attr(session.define_attribute("message", attrfmt_string), "") {}

void log_record_builder::add_severity(unsigned int severity) {
	_attrs.append(_severity_attr = unsigned_integer_attribute(_severity_attr.attrid(), severity));
}

void log_record_builder::add_facility(unsigned int facility) {
	_attrs.append(_facility_attr = unsigned_integer_attribute(_facility_attr.attrid(), facility));
}

void log_record_builder::add_version(unsigned int version) {
	_attrs.append(_version_attr = unsigned_integer_attribute(_version_attr.attrid(), version));
}

void log_record_builder::add_timestamp(const std::string& timestamp) {
	_attrs.append(_timestamp_attr = string_attribute(_timestamp_attr.attrid(), timestamp));
}

void log_record_builder::add_hostname(const std::string& hostname) {
	_attrs.append(_hostname_attr = string_attribute(_hostname_attr.attrid(), hostname));
}

void log_record_builder::add_appname(const std::string& appname) {
	_attrs.append(_appname_attr = string_attribute(_appname_attr.attrid(), appname));
}

void log_record_builder::add_procid(const std::string& procid) {
	_attrs.append(_procid_attr = string_attribute(_procid_attr.attrid(), procid));
}

void log_record_builder::add_msgid(const std::string& msgid) {
	_attrs.append(_msgid_attr = string_attribute(_msgid_attr.attrid(), msgid));
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