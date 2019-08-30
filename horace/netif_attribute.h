// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NETIF_ATTRIBUTE
#define LIBHOLMES_HORACE_NETIF_ATTRIBUTE

#include <string>

#include "horace/attribute.h"
#include "horace/attribute_list.h"
#include "horace/ifname_attribute.h"
#include "horace/ifindex_attribute.h"

namespace horace {

/** A class to represent a network interface attribute. */
class netif_attribute:
	public attribute {
private:
	/** A list of sub-attributes to describe the interface. */
	attribute_list _attrs;

	/** The interface name attribute. */
	const ifname_attribute* _ifname_attr;

	/** The interface index attribute. */
	const ifindex_attribute* _ifindex_attr;
public:
	/** Construct netif attribute.
	 * The current implementation presumes that the hardware address
	 * is an EUI.
	 * @param ifindex the interface index
	 * @param ifname the interface name
	 * @param linktype the linktype, or -1 if not known
	 * @param hwaddr the hardware address, or the empty string
	 *  if not known
	 */
	netif_attribute(unsigned int ifindex, const std::string& ifname,
		int linktype, std::string hwaddr);

	/** Construct netif attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	netif_attribute(octet_reader& in, size_t length);

	/** Get the list of sub-attributes which describe the interface.
	 * @return the list of sub-attributes
	 */
	const attribute_list& attributes() const {
		return _attrs;
	}

	/** Get the interface name attribute.
	 * return the ifname attribute
	 */
	const ifname_attribute& ifname() const {
		return *_ifname_attr;
	}

	/** Get the interface index attribute.
	 * return the ifindex attribute
	 */
	const ifindex_attribute& ifindex() const {
		return *_ifindex_attr;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
