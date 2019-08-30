// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "horace/horace_error.h"
#include "horace/relative_timestamp_attribute.h"

namespace horace {

relative_timestamp_attribute::relative_timestamp_attribute(
	octet_reader& in, int type, size_t length):
	timestamp_attribute(type),
	_ts(in.read_unsigned(length)) {

	if  ((type != ATTR_TIMESTAMP_S) && (type != ATTR_TIMESTAMP_MS) &&
		(type != ATTR_TIMESTAMP_US) && (type != ATTR_TIMESTAMP_NS)) {

		throw horace_error(
			"invalid type for relative timestamp attribute");
	}
}

relative_timestamp_attribute::relative_timestamp_attribute(
	uint64_t value, int type):
	timestamp_attribute(type),
	_ts(value) {
}

uint64_t relative_timestamp_attribute::unit_divisor() const {
	switch (type()) {
	case ATTR_TIMESTAMP_S:
		return 1;
	case ATTR_TIMESTAMP_MS:
		return 1000;
	case ATTR_TIMESTAMP_US:
		return 1000000;
	case ATTR_TIMESTAMP_NS:
		return 1000000000;
	default:
		throw horace_error(
			"invalid type for relative timestamp attribute");
	}
}

std::string relative_timestamp_attribute::unit_name() const {
	switch (type()) {
	case ATTR_TIMESTAMP_S:
		return "s";
	case ATTR_TIMESTAMP_MS:
		return "ms";
	case ATTR_TIMESTAMP_US:
		return "us";
	case ATTR_TIMESTAMP_NS:
		return "ns";
	default:
		throw horace_error(
			"invalid type for relative timestamp attribute");
	}
}

size_t relative_timestamp_attribute::length() const {
	size_t len = 1;
	uint64_t rem = _ts >> 8;
	while (rem) {
		len += 1;
		rem >>= 8;
	}
	return len;
}

void relative_timestamp_attribute::write(std::ostream& out) const {
	out << "timestamp(+" << std::dec << _ts << unit_name() << ")";
}

void relative_timestamp_attribute::write(octet_writer& out) const {
	int len = length();
	out.write_signed_base128(type());
	out.write_unsigned_base128(len);
	out.write_unsigned(_ts, len);
}

} /* namespace horace */
