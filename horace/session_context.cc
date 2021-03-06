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

/** The reserved attribute labels applicable to all sessions,
 * indexed by attribute ID. */
const std::map<int, std::string> _reserved_attr_labels = {
	{ attrid_protocol, "protocol" },
	{ attrid_attr_def, "attr_def" },
	{ attrid_attr_id, "attr_id" },
	{ attrid_attr_label, "attr_label" },
	{ attrid_chan_def, "chan_def" },
	{ attrid_chan_id, "chan_id" },
	{ attrid_chan_label, "chan_label" },
	{ attrid_type, "content_type" },
	{ attrid_source, "source" },
	{ attrid_seqnum, "seqnum" },
	{ attrid_ts, "ts" },
	{ attrid_time_system, "time_system" },
	{ attrid_message, "message" },
	{ attrid_hash, "hash" },
	{ attrid_hash_alg, "hash_alg" },
	{ attrid_sig, "hash_sig" },
	{ attrid_sig_alg, "hash_sig_alg" },
	{ attrid_sig_pubkey, "hash_sig_pubkey" }};

/** The reserved attribute formats applicable to all sessions,
 * indexed by attribute ID. */
const std::map<int, int> _reserved_attr_types = {
	{ attrid_protocol, type_unsigned_integer },
	{ attrid_attr_def, type_compound },
	{ attrid_attr_id, type_signed_integer },
	{ attrid_attr_label, type_string },
	{ attrid_chan_def, type_compound },
	{ attrid_chan_id, type_signed_integer },
	{ attrid_chan_label, type_string },
	{ attrid_type, type_unsigned_integer },
	{ attrid_source, type_string },
	{ attrid_seqnum, type_unsigned_integer },
	{ attrid_ts, type_timestamp },
	{ attrid_time_system, type_string },
	{ attrid_message, type_string },
	{ attrid_hash, type_binary },
	{ attrid_hash_alg, type_string },
	{ attrid_sig, type_binary },
	{ attrid_sig_alg, type_string },
	{ attrid_sig_pubkey, type_binary }};

/** The reserved channel labels applicable to all sessions,
 * indexed by channel ID. */
std::map<int, std::string> _reserved_chan_labels = {
	{ channel_error, "error" },
	{ channel_session, "session" },
	{ channel_sync, "sync" }};

} /* anonymous namespace */

void session_context::handle_attr_def(const compound_attribute& attr) {
	int64_t attrid = attr.content().find_one<signed_integer_attribute>(
		attrid_attr_id).content();
	std::string attrlabel = attr.content().find_one<string_attribute>(
		attrid_attr_label).content();
	uint64_t type = attr.content().find_one<unsigned_integer_attribute>(
		attrid_type).content();
	_attr_labels[attrid] = attrlabel;
	_attr_types[attrid] = type;
}

void session_context::handle_channel_def(const compound_attribute& attr) {
	int64_t chanid = attr.content().find_one<signed_integer_attribute>(
		attrid_chan_id).content();
	std::string label = attr.content().find_one<string_attribute>(
		attrid_chan_label).content();
	_chan_labels[chanid] = label;
}

const std::string& session_context::get_attr_label(int attrid) {
	if (attrid < 0) {
		auto f = _reserved_attr_labels.find(attrid);
		if (f != _reserved_attr_labels.end()) {
			return f->second;
		}
	} else {
		auto f = _attr_labels.find(attrid);
		if (f != _attr_labels.end()) {
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

const std::string& session_context::get_channel_label(int channel_id) {
	if (channel_id < 0) {
		auto f = _reserved_chan_labels.find(channel_id);
		if (f != _reserved_chan_labels.end()) {
			return f->second;
		}
	} else {
		auto f = _chan_labels.find(channel_id);
		if (f != _chan_labels.end()) {
			return f->second;
		}
	}
	throw horace_error(std::string("unrecognised channel ID") +
		std::to_string(channel_id));
}

} /* namespace horace */
