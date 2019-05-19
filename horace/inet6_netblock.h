// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_INET6_NETBLOCK
#define LIBHOLMES_HORACE_INET6_NETBLOCK

#include <string>

namespace horace {

/** A class to represent an IPv6 netblock. */
class inet6_netblock {
private:
	/** The IPv6 prefix. */
	char _prefix[16];

	/** The prefix length, in bits. */
	unsigned int _prefix_length;
public:
	/** Construct netblock from string.
	 * @param str the netblock as a string
	 */
	inet6_netblock(const std::string& str);

	/** Get the prefix for this netblock.
	 * @return the prefix
	 */
	const void* prefix() const {
		return _prefix;
	}

	/** Get the prefix length for this netblock.
	 * @return the prefix length, in bits
	 */
	unsigned int prefix_length() const {
		return _prefix_length;
	}
};

} /* namespace horace */

#endif
