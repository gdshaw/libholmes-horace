// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace_error.h"
#include "netif_attribute.h"
#include "ifname_attribute.h"
#include "ifindex_attribute.h"
#include "linktype_attribute.h"
#include "eui_attribute.h"

namespace horace {

netif_attribute::netif_attribute(unsigned int ifindex,
	const std::string& ifname, int linktype, std::string hwaddr) {

	std::unique_ptr<ifname_attribute> ifname_attr =
		std::make_unique<ifname_attribute>(ifname);
	std::unique_ptr<ifindex_attribute> ifindex_attr =
		std::make_unique<ifindex_attribute>(ifindex);
	_ifname_attr = ifname_attr.get();
	_ifindex_attr = ifindex_attr.get();

	_attrs.append(std::move(ifname_attr));
	_attrs.append(std::move(ifindex_attr));
	if (linktype != -1) {
		_attrs.append(std::make_unique<linktype_attribute>(linktype));
	}
	if (!hwaddr.empty()) {
		_attrs.append(std::make_unique<eui_attribute>(hwaddr));
	}
}

netif_attribute::netif_attribute(octet_reader& in, size_t length):
	_attrs(in, length),
	_ifname_attr(0),
	_ifindex_attr(0) {

	for (auto attr : _attrs.attributes()) {
		if (const ifname_attribute* ifname_attr =
			dynamic_cast<const ifname_attribute*>(attr)) {

			if (_ifname_attr) {
				throw horace_error(
					"duplicate ifname attribute in netif attribute");
			}
			_ifname_attr = ifname_attr;
		} else if (const ifindex_attribute* ifindex_attr =
			dynamic_cast<const ifindex_attribute*>(attr)) {

			if (_ifindex_attr) {
				throw horace_error(
					"duplicate ifindex attribute in netif attribute");
			}
			_ifindex_attr = ifindex_attr;
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

int netif_attribute::type() const {
	return ATTR_NETIF;
}

size_t netif_attribute::length() const {
	return _attrs.length();
}

void netif_attribute::write(std::ostream& out) const {
	out << "netif" << _attrs;
}

void netif_attribute::write(octet_writer& out) const {
	out.write_base128(type());
	out.write_base128(length());
	_attrs.write(out);
}

} /* namespace horace */
