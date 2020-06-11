// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/session_builder.h"

#include "clock_record_builder.h"

namespace horace {

clock_record_builder::clock_record_builder(session_builder& session, int channel):
	_channel(channel),
	_ts_attr(attrid_ts, 0, 0),
	_sync_attr(session.define_attribute("clock_sync", type_boolean), 0),
	_tzoffset_attr(session.define_attribute("tz_offset", type_signed_integer), 0),
	_tzabbrev_attr(session.define_attribute("tz_abbrev", type_string), ""),
	_tzname_attr(session.define_attribute("tz_name", type_string), "") {}

const struct timespec& clock_record_builder::add_ts() {
	_attrs.insert(_ts_attr = timestamp_attribute(_ts_attr.attrid()));
	return _ts_attr.content();
}

void clock_record_builder::add_sync(bool sync) {
	_attrs.insert(_sync_attr = boolean_attribute(_sync_attr.attrid(), sync));
}

void clock_record_builder::add_tzoffset(long tzoffset) {
	_attrs.insert(_tzoffset_attr = signed_integer_attribute(_tzoffset_attr.attrid(), tzoffset));
}

void clock_record_builder::add_tzabbrev(const std::string& tzabbrev) {
	_attrs.insert(_tzabbrev_attr = string_attribute(_tzabbrev_attr.attrid(), tzabbrev));
}

void clock_record_builder::add_tzname(const std::string& tzname) {
	_attrs.insert(_tzname_attr = string_attribute(_tzname_attr.attrid(), tzname));
}

void clock_record_builder::add_ntp(const attribute& ntp_attr) {
	_attrs.insert(ntp_attr);
}

const record& clock_record_builder::build() {
	_built = record(_channel, std::move(_attrs));
	return _built;
}

} /* namespace horace */
