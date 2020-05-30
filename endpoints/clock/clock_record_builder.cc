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
	_sync_attr(session.define_attribute("clock_sync", attrfmt_boolean), 0) {}

void clock_record_builder::add_sync(bool sync) {
	_attrs.append(_sync_attr = boolean_attribute(_sync_attr.attrid(), sync));
}

const record& clock_record_builder::build() {
	_attrs.append(_ts_attr = timestamp_attribute(_ts_attr.attrid()));
	_built = record(_channel, std::move(_attrs));
	return _built;
}

} /* namespace horace */
