// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/signed_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/record.h"
#include "horace/session_context.h"

namespace horace {
namespace {

/** The reserved attribute names applicable to all sessions,
 * indexed by attribute ID. */
const std::map<int, std::string> _reserved_attr_names = {
	{ attrid_attr_def, "attributes" },
	{ attrid_attr_id, "id" },
	{ attrid_attr_name, "name" },
	{ attrid_attr_type, "type" },
	{ attrid_channel_def, "channels" },
	{ attrid_channel_num, "channel" },
	{ attrid_channel_label, "label" },
	{ attrid_source, "source" },
	{ attrid_seqnum, "seqnum" },
	{ attrid_ts_begin, "ts_begin" },
	{ attrid_ts_end, "ts_end" },
	{ attrid_hash, "hash" },
	{ attrid_hash_alg, "hash_alg" },
	{ attrid_sig, "hash_sig" },
	{ attrid_sig_alg, "hash_sig_alg" },
	{ attrid_sig_pubkey, "hash_sig_pubkey" }};

/** The reserved attribute types applicable to all sessions,
 * indexed by attribute ID. */
const std::map<int, int> _reserved_attr_types = {
	{ attrid_attr_def, attrtype_compound },
	{ attrid_attr_id, attrtype_signed_integer },
	{ attrid_attr_name, attrtype_string },
	{ attrid_attr_type, attrtype_unsigned_integer },
	{ attrid_channel_def, attrtype_compound },
	{ attrid_channel_num, attrtype_signed_integer },
	{ attrid_channel_label, attrtype_string },
	{ attrid_source, attrtype_string },
	{ attrid_seqnum, attrtype_unsigned_integer },
	{ attrid_ts_begin, attrtype_timestamp },
	{ attrid_ts_end, attrtype_timestamp },
	{ attrid_hash, attrtype_binary },
	{ attrid_hash_alg, attrtype_string },
	{ attrid_sig, attrtype_binary },
	{ attrid_sig_alg, attrtype_string },
	{ attrid_sig_pubkey, attrtype_binary }};

/** The reserved channel labels applicable to all sessions,
 * indexed by channel number. */
std::map<int, std::string> _reserved_chan_labels = {
	{ channel_error, "error" },
	{ channel_session, "session" },
	{ channel_sync, "sync" }};

} /* anonymous namespace */

void session_context::handle_attr_def(const compound_attribute& attr) {
	int64_t attrid = attr.content().find_one<signed_integer_attribute>(
		attrid_attr_id).content();
	std::string attrname = attr.content().find_one<string_attribute>(
		attrid_attr_name).content();
	uint64_t attrtype = attr.content().find_one<unsigned_integer_attribute>(
		attrid_attr_type).content();
	_attr_names[attrid] = attrname;
	_attr_types[attrid] = attrtype;
}

void session_context::handle_channel_def(const compound_attribute& attr) {
	int64_t num = attr.content().find_one<signed_integer_attribute>(
		attrid_channel_num).content();
	std::string label = attr.content().find_one<string_attribute>(
		attrid_channel_label).content();
	_chan_labels[num] = label;
}

const std::string& session_context::get_attr_name(int attrid) {
	if (attrid < 0) {
		auto f = _reserved_attr_names.find(attrid);
		if (f != _reserved_attr_names.end()) {
			return f->second;
		}
	} else {
		auto f = _attr_names.find(attrid);
		if (f != _attr_names.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised attribute ID ") +
		std::to_string(attrid));
}

int session_context::get_attr_type(int attrid) {
	if (attrid < 0) {
		auto f = _reserved_attr_types.find(attrid);
		if (f != _reserved_attr_types.end()) {
			return f->second;
		}
	} else {
		auto f = _attr_types.find(attrid);
		if (f != _attr_types.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised attribute ID ") +
		std::to_string(attrid));
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
