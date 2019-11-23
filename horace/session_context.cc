// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/session_context.h"

namespace horace {

session_context::session_context() {
	_attr_labels[attr_type_def] = "attributes";
	_attr_labels[attr_type_code] = "code";
	_attr_labels[attr_type_label] = "label";
	_attr_labels[attr_type_format] = "format";
	_attr_labels[attr_channel_def] = "channels";
	_attr_labels[attr_channel_num] = "channel";
	_attr_labels[attr_channel_label] = "label";
	_attr_labels[attr_source] = "source";
	_attr_labels[attr_timestamp] = "timestamp";
	_attr_labels[attr_seqnum] = "seqnum";

	_attr_formats[attr_type_def] = attr_format_compound;
	_attr_formats[attr_type_code] = attr_format_unsigned_integer;
	_attr_formats[attr_type_label] = attr_format_string;
	_attr_formats[attr_type_format] = attr_format_unsigned_integer;
	_attr_formats[attr_channel_def] = attr_format_compound;
	_attr_formats[attr_channel_num] = attr_format_unsigned_integer;
	_attr_formats[attr_channel_label] = attr_format_string;
	_attr_formats[attr_source] = attr_format_string;
	_attr_formats[attr_timestamp] = attr_format_timestamp;
	_attr_formats[attr_seqnum] = attr_format_unsigned_integer;
}

void session_context::handle_attr_type_def(const compound_attribute& attr) {
	uint64_t code = attr.content().find_one<unsigned_integer_attribute>(
		attr_type_code).content();
	std::string label = attr.content().find_one<string_attribute>(
		attr_type_label).content();
	uint64_t format = attr.content().find_one<unsigned_integer_attribute>(
		attr_type_format).content();
	_attr_labels[code] = label;
	_attr_formats[code] = format;
}

void session_context::handle_channel_def(const compound_attribute& attr) {
	uint64_t num = attr.content().find_one<unsigned_integer_attribute>(
		attr_channel_num).content();
	std::string label = attr.content().find_one<string_attribute>(
		attr_channel_label).content();
	_chan_labels[num] = label;
}

const std::string& session_context::get_attr_label(int type_code) {
	auto f = _attr_labels.find(type_code);
	if (f == _attr_labels.end()) {
		throw horace_error(std::string("unrecognised attribute type ") +
			std::to_string(type_code));
	}
	return f->second;
}

int session_context::get_attr_format(int type_code) {
	auto f = _attr_formats.find(type_code);
	if (f == _attr_formats.end()) {
		throw horace_error(std::string("unrecognised attribute type ") +
			std::to_string(type_code));
	}
	return f->second;
}

const std::string& session_context::get_channel_label(int channel_num) {
	auto f = _chan_labels.find(channel_num);
	if (f == _chan_labels.end()) {
		throw horace_error(std::string("unrecognised channel number") +
			std::to_string(channel_num));
	}
	return f->second;
}

} /* namespace horace */
