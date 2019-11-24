// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/session_builder.h"

namespace horace {

session_builder::session_builder(const std::string& source_id):
	_source_id(source_id),
	_srecb(channel_session),
	_attr_count(0),
	_chan_count(0) {

	_srecb.append(std::make_unique<string_attribute>(
		attr_source, source_id));
	_srecb.append(std::make_unique<timestamp_attribute>
		(attr_ts_begin));
}

int session_builder::define_attribute_type(const std::string& label,
	int format) {

	attribute_list attrs;
	attrs.append(std::make_unique<unsigned_integer_attribute>(
		attr_type_code, _attr_count));
	attrs.append(std::make_unique<string_attribute>(
		attr_type_label, label));
	attrs.append(std::make_unique<unsigned_integer_attribute>(
		attr_type_format, format));
	_srecb.append(std::make_unique<compound_attribute>(
		attr_type_def, std::move(attrs)));
	return _attr_count++;
}

int session_builder::define_channel(const std::string& label,
	attribute_list&& attrs) {

	attrs.append(std::make_unique<unsigned_integer_attribute>(
		attr_channel_num, _chan_count));
	attrs.append(std::make_unique<string_attribute>(
		attr_channel_label, label));
	_srecb.append(std::make_unique<compound_attribute>(
		attr_channel_def, std::move(attrs)));
	return _chan_count;
}

std::unique_ptr<record> session_builder::build() {
	return _srecb.build();
}

} /* namespace horace */
