// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/record.h"
#include "horace/session_builder.h"

namespace horace {

session_builder::session_builder(const std::string& source_id):
	_source_id(source_id),
	_attr_count(0),
	_chan_count(0) {

	_attributes.append(std::make_unique<string_attribute>(
		attrid_source, source_id));
	_attributes.append(std::make_unique<timestamp_attribute>
		(attrid_ts_begin));
}

int session_builder::define_attribute(const std::string& name,
	int type) {

	attribute_list subattrs;
	subattrs.append(std::make_unique<unsigned_integer_attribute>(
		attrid_attr_id, _attr_count));
	subattrs.append(std::make_unique<string_attribute>(
		attrid_attr_name, name));
	subattrs.append(std::make_unique<unsigned_integer_attribute>(
		attrid_attr_type, type));
	_attributes.append(std::make_unique<compound_attribute>(
		attrid_attr_def, std::move(subattrs)));
	return _attr_count++;
}

int session_builder::define_channel(const std::string& label,
	attribute_list&& subattrs) {

	subattrs.append(std::make_unique<unsigned_integer_attribute>(
		attrid_channel_num, _chan_count));
	subattrs.append(std::make_unique<string_attribute>(
		attrid_channel_label, label));
	_attributes.append(std::make_unique<compound_attribute>(
		attrid_channel_def, std::move(subattrs)));
	return _chan_count;
}

std::unique_ptr<record> session_builder::build() {
	return std::make_unique<record>(channel_session, _attributes);
}

} /* namespace horace */
