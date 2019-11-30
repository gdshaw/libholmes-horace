// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/record.h"
#include "horace/session_context.h"

namespace horace {
namespace {

/** The reserved attribute labels applicable to all sessions,
 * indexed by code. */
const std::map<int, std::string> _reserved_attr_labels = {
	{ attr_type_def, "attributes" },
	{ attr_type_code, "code" },
	{ attr_type_label, "label" },
	{ attr_type_format, "format" },
	{ attr_channel_def, "channels" },
	{ attr_channel_num, "channel" },
	{ attr_channel_label, "label" },
	{ attr_source, "source" },
	{ attr_seqnum, "seqnum" },
	{ attr_ts_begin, "ts_begin" },
	{ attr_ts_end, "ts_end" }};

/** The reserved attribute formats applicable to all sessions,
 * indexed by code. */
const std::map<int, int> _reserved_attr_formats = {
	{ attr_type_def, attr_format_compound },
	{ attr_type_code, attr_format_unsigned_integer },
	{ attr_type_label, attr_format_string },
	{ attr_type_format, attr_format_unsigned_integer },
	{ attr_channel_def, attr_format_compound },
	{ attr_channel_num, attr_format_unsigned_integer },
	{ attr_channel_label, attr_format_string },
	{ attr_source, attr_format_string },
	{ attr_seqnum, attr_format_unsigned_integer },
	{ attr_ts_begin, attr_format_timestamp },
	{ attr_ts_end, attr_format_timestamp }};

/** The reserved channel labels applicable to all sessions,
 * indexed by channel number. */
std::map<int, std::string> _reserved_chan_labels = {
	{ channel_error, "error" },
	{ channel_session, "session" },
	{ channel_sync, "sync" }};

} /* anonymous namespace */

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
	if (type_code < 0) {
		auto f = _reserved_attr_labels.find(type_code);
		if (f != _reserved_attr_labels.end()) {
			return f->second;
		}
	} else {
		auto f = _attr_labels.find(type_code);
		if (f != _attr_labels.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised attribute type ") +
		std::to_string(type_code));
}

int session_context::get_attr_format(int type_code) {
	if (type_code < 0) {
		auto f = _reserved_attr_formats.find(type_code);
		if (f != _reserved_attr_formats.end()) {
			return f->second;
		}
	} else {
		auto f = _attr_formats.find(type_code);
		if (f != _attr_formats.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised attribute type ") +
		std::to_string(type_code));
}

const std::string& session_context::get_channel_label(int channel_num) {
	if (channel_num < 0) {
		auto f = _reserved_chan_labels.find(channel_num);
		if (f != _reserved_chan_labels.end()) {
			return f->second;
		}
	} else {
		auto f = _chan_labels.find(channel_num);
		if (f != _chan_labels.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised channel number") +
		std::to_string(channel_num));
}

} /* namespace horace */
