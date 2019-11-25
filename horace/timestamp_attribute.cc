// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/timestamp_attribute.h"

namespace horace {

timestamp_attribute::timestamp_attribute(int type,
	size_t length, octet_reader& in):
	attribute(type) {

	if (length < 5) {
		throw horace_error(
			"invalid length for timestamp attribute");
	}
	_content.tv_sec = in.read_unsigned(length - 4);
	_content.tv_nsec = in.read_unsigned(4);
}

timestamp_attribute::timestamp_attribute(int type):
	attribute(type) {

	if (clock_gettime(CLOCK_REALTIME, &_content) == -1) {
		throw libc_error();
	}
}

timestamp_attribute::timestamp_attribute(int type, time_t sec, long nsec):
	attribute(type) {

	_content.tv_sec = sec;
	_content.tv_nsec = nsec;
}

timestamp_attribute::timestamp_attribute(int type, const timespec& content):
	attribute(type),
	_content(content) {
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
	return std::make_unique<timestamp_attribute>(type(), _content);
}

void timestamp_attribute::write(std::ostream& out) const {
	out << "attr" << type() << "(" <<
		_content.tv_sec << "." <<
		std::setfill('0') << std::setw(9) << _content.tv_nsec << ")";
}

void timestamp_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(type());
	out.write_unsigned_base128(len);
	out.write_unsigned(_content.tv_sec, len - 4);
	out.write_unsigned(_content.tv_nsec, 4);
}

} /* namespace horace */
