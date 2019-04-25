// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/packet_ref_attribute.h"

namespace horace {

packet_ref_attribute::packet_ref_attribute(const void* content, size_t length):
	_content(reinterpret_cast<const char*>(content)),
	_length(length) {}

void packet_ref_attribute::write(std::ostream& out) const {
	std::cout << "packet(" << std::hex << std::setfill('0');
	for (size_t i = 0; i != _length; ++i) {
		out << std::setw(2) << (_content[i] & 0xff);
	}
	out << std::dec << ")";
}

void packet_ref_attribute::write(octet_writer& out) const {
	out.write_base128(type());
	out.write_base128(_length);
	out.write(_content, _length);
}

} /* namespace horace */
