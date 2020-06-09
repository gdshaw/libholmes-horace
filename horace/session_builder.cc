// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/hash.h"
#include "horace/keypair.h"
#include "horace/binary_attribute.h"
#include "horace/string_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/signed_integer_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/record.h"
#include "horace/session_builder.h"

namespace horace {

session_builder::session_builder(const std::string& srcid,
	const std::string& time_system):
	_srcid(srcid),
	_attr_count(0),
	_chan_count(0) {

	if (protocol_version) {
		_attributes.insert(std::make_unique<unsigned_integer_attribute>(
			attrid_protocol, *protocol_version));
	}
	_attributes.insert(std::make_unique<string_attribute>(
		attrid_time_system, time_system));
	_attributes.insert(std::make_unique<string_attribute>(
		attrid_source, srcid));
	_attributes.insert(std::make_unique<timestamp_attribute>
		(attrid_ts));
}

int session_builder::define_attribute(const std::string& label, int type) {
	attribute_list subattrs;
	subattrs.insert(std::make_unique<signed_integer_attribute>(
		attrid_attr_id, _attr_count));
	subattrs.insert(std::make_unique<string_attribute>(
		attrid_attr_label, label));
	subattrs.insert(std::make_unique<unsigned_integer_attribute>(
		attrid_type, type));
	_attributes.insert(std::make_unique<compound_attribute>(
		attrid_attr_def, std::move(subattrs)));
	return _attr_count++;
}

int session_builder::define_channel(const std::string& label,
	attribute_list&& subattrs) {

	subattrs.insert(std::make_unique<signed_integer_attribute>(
		attrid_chan_id, _chan_count));
	subattrs.insert(std::make_unique<string_attribute>(
		attrid_chan_label, label));
	_attributes.insert(std::make_unique<compound_attribute>(
		attrid_chan_def, std::move(subattrs)));
	return _chan_count++;
}

void session_builder::define_hash(const hash& hashfn) {
	_attributes.insert(std::make_unique<string_attribute>(
		attrid_hash_alg, hashfn.algorithm()));
}

void session_builder::define_keypair(const keypair& kp) {
	_attributes.insert(std::make_unique<string_attribute>(
		attrid_sig_alg, kp.algorithm()));
	std::basic_string<unsigned char> pubkey = kp.public_key();
	_attributes.insert(std::make_unique<binary_attribute>(
		attrid_sig_pubkey, pubkey.length(), pubkey.data()));
}

std::unique_ptr<record> session_builder::build() {
	return std::make_unique<record>(channel_session, _attributes);
}

} /* namespace horace */
