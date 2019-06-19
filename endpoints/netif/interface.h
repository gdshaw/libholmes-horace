// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_INTERFACE
#define LIBHOLMES_HORACE_INTERFACE

#include <string>

namespace horace {

/** A class to represent a network interface. */
class interface {
public:
	static const int linktype_ethernet = 1;
private:
	/** The interface index, or 0 for any interface. */
	unsigned int _ifindex;

	/** The linktype. */
	int _linktype;

	/** The hardware address, or the empty string if not known
	 * or not applicable. */
	std::string _hwaddr;
public:
	/** Construct interface object for any interface. */
	interface();

	/** Construct interface object from name.
	 * @param ifname the interface name
	 */
	explicit interface(const std::string& ifname);

	/** Get interface index.
	 * @return the interface index, or 0 for any index
	 */
	operator int() const {
		return _ifindex;
	}

	/** Get the linktype.
	 * @return the linktype, or -1 if not known
	 */
	int linktype() const {
		return _linktype;
	}

	/** Get the hardware address.
	 * @return the hardware address, or the empty string if not known
	 *  or not applicable
	 */
	const std::string& hwaddr() const {
		return _hwaddr;
	}
};

} /* namespace horace */

#endif
