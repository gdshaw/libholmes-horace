// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "netif_attribute.h"
#include "ifname_attribute.h"
#include "ifindex_attribute.h"

namespace horace {

netif_attribute::netif_attribute(unsigned int ifindex,
	const std::string& ifname) {

	_attributes.append(std::make_unique<ifindex_attribute>(ifindex));
	_attributes.append(std::make_unique<ifname_attribute>(ifname));
}

netif_attribute::netif_attribute(octet_reader& in, size_t length):
	_attributes(in, length) {}

int netif_attribute::type() const {
	return ATTR_NETIF;
}

size_t netif_attribute::length() const {
	return _attributes.length();
}

void netif_attribute::write(std::ostream& out) const {
	out << "netif" << _attributes;
}

void netif_attribute::write(octet_writer& out) const {
	out.write_base128(type());
	out.write_base128(length());
	_attributes.write(out);
}

} /* namespace horace */
