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
	_priority_attr(session.define_attribute("syslog_priority", type_unsigned_integer), 0),
	_version_attr(session.define_attribute("syslog_version", type_unsigned_integer), 0),
	_timestamp_attr(session.define_attribute("syslog_timestamp", type_string), ""),
	_hostname_attr(session.define_attribute("hostname", type_string), ""),
	_appname_attr(session.define_attribute("syslog_appname", type_string), ""),
	_procid_attr(session.define_attribute("syslog_procid", type_string), ""),
	_msgid_attr(session.define_attribute("syslog_msgid", type_string), ""),
	_sd_attr(session.define_attribute("syslog_sd", type_string), ""),
	_message_attr(session.define_attribute("message", type_string), "") {}

void log_record_builder::add_priority(unsigned int priority) {
	_attrs.append(_priority_attr = unsigned_integer_attribute(_priority_attr.attrid(), priority));
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

void log_record_builder::add_structured_data(const std::string& sd) {
	_attrs.append(_sd_attr = string_attribute(_sd_attr.attrid(), sd));
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
