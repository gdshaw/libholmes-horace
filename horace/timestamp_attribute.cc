// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <limits>
#include <iomanip>

#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/timestamp_attribute.h"

namespace horace {

timestamp_attribute::timestamp_attribute(int attrid,
	size_t length, octet_reader& in):
	attribute(attrid) {

	if ((length < 5) || (length > 12)) {
		throw horace_error(
			"invalid length for timestamp attribute");
	}
	uint64_t sec = in.read_unsigned(length - 4);
	uint32_t nsec = in.read_unsigned(4);
	if (sec > std::numeric_limits<time_t>::max()) {
		throw horace_error(
			"invalid sec field in timestamp attribute");
	}
	if (nsec >= 2000000000) {
		throw horace_error(
			"invalid nsec field in timestamp attribute");
	}
	_content.tv_sec = sec;
	_content.tv_nsec = nsec;
}

timestamp_attribute::timestamp_attribute(int attrid):
	attribute(attrid) {

	if (clock_gettime(CLOCK_REALTIME, &_content) == -1) {
		throw libc_error();
	}
}

timestamp_attribute::timestamp_attribute(int attrid, time_t sec, long nsec):
	attribute(attrid) {

	if (nsec >= 2000000000) {
		throw horace_error(
			"invalid nsec field in timestamp attribute");
	}
	_content.tv_sec = sec;
	_content.tv_nsec = nsec;
}

timestamp_attribute::timestamp_attribute(int attrid, const timespec& content):
	attribute(attrid),
	_content(content) {

	if (_content.tv_nsec >= 2000000000) {
		throw horace_error(
			"invalid nsec field in timestamp attribute");
	}
}

bool timestamp_attribute::operator==(const attribute& that) const {
	if (this->attrid() != that.attrid()) {
		return false;
	}
	const timestamp_attribute* _that =
		dynamic_cast<const timestamp_attribute*>(&that);
	if (!_that) {
		return false;
	}
	return ((_content.tv_sec == _that->_content.tv_sec) &&
		(_content.tv_nsec == _that->_content.tv_nsec));
}

size_t timestamp_attribute::length() const {
	size_t len = 5;
	uint64_t rem = _content.tv_sec >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

std::unique_ptr<attribute> timestamp_attribute::clone() const {
	return std::make_unique<timestamp_attribute>(attrid(), _content);
}

void timestamp_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << "(" <<
		_content.tv_sec << "." <<
		std::setfill('0') << std::setw(9) << _content.tv_nsec << ")";
}

void timestamp_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(attrid());
	out.write_unsigned_base128(len);
	out.write_unsigned(_content.tv_sec, len - 4);
	out.write_unsigned(_content.tv_nsec, 4);
}

} /* namespace horace */
