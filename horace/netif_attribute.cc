// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace_error.h"
#include "netif_attribute.h"
#include "eui_attribute.h"

namespace horace {

netif_attribute::netif_attribute(unsigned int ifindex,
	const std::string& ifname, int linktype, std::string hwaddr):
	attribute(ATTR_NETIF) {

	std::unique_ptr<string_attribute> ifname_attr =
		std::make_unique<string_attribute>(attribute::ATTR_IFNAME, ifname);
	std::unique_ptr<unsigned_integer_attribute> ifindex_attr =
		std::make_unique<unsigned_integer_attribute>(attribute::ATTR_IFINDEX, ifindex);
	_ifname_attr = ifname_attr.get();
	_ifindex_attr = ifindex_attr.get();

	_attrs.append(std::move(ifname_attr));
	_attrs.append(std::move(ifindex_attr));
	if (linktype != -1) {
		_attrs.append(std::make_unique<unsigned_integer_attribute>(
			attribute::ATTR_LINKTYPE, linktype));
	}
	if (!hwaddr.empty()) {
		_attrs.append(std::make_unique<eui_attribute>(hwaddr));
	}
}

netif_attribute::netif_attribute(octet_reader& in, size_t length):
	attribute(ATTR_NETIF),
	_attrs(in, length),
	_ifname_attr(0),
	_ifindex_attr(0) {

	for (auto attr : _attrs.attributes()) {
		if (attr->type() == attribute::ATTR_IFNAME) {
			if (_ifname_attr) {
				throw horace_error(
					"duplicate ifname attribute in netif attribute");
			}
			_ifname_attr = &dynamic_cast<const string_attribute&>(*attr);
		} else if (attr->type() == attribute::ATTR_IFINDEX) {
			if (_ifindex_attr) {
				throw horace_error(
					"duplicate ifindex attribute in netif attribute");
			}
			_ifindex_attr = &dynamic_cast<const unsigned_integer_attribute&>(*attr);
		}
	}

	if (!_ifname_attr) {
		throw horace_error(
			"missing ifname attribute in netif attribute");
	}
	if (!_ifindex_attr) {
		throw horace_error(
			"missing ifindex attribute in netif attribute");
	}
}

size_t netif_attribute::length() const {
	return _attrs.length();
}

void netif_attribute::write(std::ostream& out) const {
	out << "netif" << _attrs;
}

void netif_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(length());
	_attrs.write(out);
}

} /* namespace horace */
