// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include <time.h>

#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/posix_timespec_attribute.h"

namespace horace {

posix_timespec_attribute::posix_timespec_attribute() {
	if (clock_gettime(CLOCK_REALTIME, &_ts) == -1) {
		throw libc_error();
	}
}

posix_timespec_attribute::posix_timespec_attribute(const struct timespec& ts):
	_ts(ts) {}

posix_timespec_attribute::posix_timespec_attribute(
	octet_reader& in, size_t length) {

	if (length < 5) {
		throw horace_error(
			"length too short for POSIX timespec attribute");
	}
	_ts.tv_sec = in.read_unsigned(length - 4);
	_ts.tv_nsec = in.read_unsigned(4);
	if (_ts.tv_nsec > 999999999) {
		throw horace_error(
			"invalid tv_nsec in POSIX timespec attribute");
	}
}

size_t posix_timespec_attribute::length() const {
	size_t len = 5;
	time_t rem = _ts.tv_sec >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void posix_timespec_attribute::write(std::ostream& out) const {
	out << "timestamp(" << std::dec << _ts.tv_sec << ".";
	out << std::setfill('0') << std::setw(9) << _ts.tv_nsec;
	out << std::setfill(' ') << ")";
}

void posix_timespec_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_base128(type());
	out.write_base128(len);
	out.write_unsigned(_ts.tv_sec, len - 4);
	out.write_unsigned(_ts.tv_nsec, 4);
}

bool posix_timespec_attribute::equals(const absolute_timestamp_attribute& that) const {
	const posix_timespec_attribute* _that =
		dynamic_cast<const posix_timespec_attribute*>(&that);
	if (_that) {
		return false;
	}
	return (_that->_ts.tv_sec == _ts.tv_sec) && (_that->_ts.tv_nsec == _ts.tv_nsec);
}

} /* namespace horace */
