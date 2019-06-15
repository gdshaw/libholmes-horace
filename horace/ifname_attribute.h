// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_IFNAME_ATTRIBUTE
#define LIBHOLMES_HORACE_IFNAME_ATTRIBUTE

#include <string>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a network interface name attribute. */
class ifname_attribute:
	public attribute {
private:
	/** The interface name. */
	std::string _ifname;
public:
	/** Construct network interface name attribute.
	 * @param ifname the interface name
	 */
	explicit ifname_attribute(const std::string& ifname);

	/** Construct ifname attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	ifname_attribute(octet_reader& in, size_t length);

	/** Get the interface name.
	 * @return the interface name
	 */
	const std::string& ifname() const {
		return _ifname;
	}

	virtual int type() const;
	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
