// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sstream>
#include <iomanip>

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

record::~record() {
	for (const attribute* attr : _owned_attributes) {
		delete attr;
	}
}

record::record(record&& that) {
	_channel = that._channel;
	_attributes.swap(that._attributes);
	_owned_attributes.swap(that._owned_attributes);
}

size_t record::length() const {
	size_t len = 0;
	for (auto&& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::signed_base128_length(attr->type());
		len += octet_writer::unsigned_base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

uint64_t record::update_seqnum(uint64_t seqnum) const {
	if (is_event()) {
		for (auto&& attr : _attributes) {
			if (attr->type() == ATTR_SEQNUM) {
				return dynamic_cast<const unsigned_integer_attribute&>(*attr).content();
			}
		}
	}
	return seqnum;
}

const attribute& record::_find_one(int type) const {
	const attribute* found = 0;
	for (auto&& attr : _attributes) {
		if (attr -> type() == type) {
			if (found) {
				throw horace_error("unexpected multiple attributes of same type");
			}
			found = attr;
		}
	}
	if (!found) {
		throw horace_error("expected attribute not found");
	}
	return *found;
}

void record::write(octet_writer& out) const {
	out.write_signed_base128(channel_number());
	out.write_unsigned_base128(length());
	for (auto&& attr : _attributes) {
		attr->write(out);
	}
}

void record::log(logger& log) const {
	if (log.enabled(logger::log_info)) {
		log_message msg(log, logger::log_info);
		msg << "unrecognised message for channel "
			<< _channel;
	}
}

std::ostream& operator<<(std::ostream& out, const record& rec) {
	out << "rec" << rec.channel_number() << "(";
	for (auto&& attr : rec.attributes()) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

bool same_session(const record& lhs, const record& rhs) {
	std::string lsource = lhs.find_one<string_attribute>(
		ATTR_SOURCE).content();
	std::string rsource = rhs.find_one<string_attribute>(
		ATTR_SOURCE).content();
	if (lsource != rsource) return false;

	struct timespec lts = lhs.find_one<timestamp_attribute>(
		ATTR_TIMESTAMP).content();
	struct timespec rts = rhs.find_one<timestamp_attribute>(
		ATTR_TIMESTAMP).content();
	if ((lts.tv_sec != rts.tv_sec) || (lts.tv_nsec != rts.tv_nsec)) {
		return false;
	}

	return true;
}

} /* namespace horace */
