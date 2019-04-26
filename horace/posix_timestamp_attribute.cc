// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/posix_timestamp_attribute.h"

namespace horace {

posix_timestamp_attribute::posix_timestamp_attribute(
	octet_reader& in, size_t length):
	_ts(in.read_unsigned(length)) {}

posix_timestamp_attribute::posix_timestamp_attribute(time_t ts):
	_ts(ts) {}

size_t posix_timestamp_attribute::length() const {
	size_t len = 1;
	time_t rem = _ts >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void posix_timestamp_attribute::write(std::ostream& out) const {
	out << "timestamp(" << std::dec << _ts << ")";
}

void posix_timestamp_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_base128(type());
	out.write_base128(len);
	out.write_unsigned(_ts, len);
}

} /* namespace horace */