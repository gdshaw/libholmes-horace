// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "ifname_attribute.h"

namespace horace {

ifname_attribute::ifname_attribute(const std::string& ifname):
	attribute(ATTR_IFNAME),
	_ifname(ifname) {}

ifname_attribute::ifname_attribute(octet_reader& in, size_t length):
	attribute(ATTR_IFNAME),
	_ifname(in.read_string(length)) {}

size_t ifname_attribute::length() const {
	return _ifname.length();
}

void ifname_attribute::write(std::ostream& out) const {
	out << "ifname(" << _ifname << ")";
}

void ifname_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(length());
	out.write_string(_ifname);
}

} /* namespace horace */
