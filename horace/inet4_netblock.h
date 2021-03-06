// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_INET4_NETBLOCK
#define LIBHOLMES_HORACE_INET4_NETBLOCK

#include <string>

namespace horace {

/** A class to represent an IPv4 netblock.
 * The textual representation accepted by this class consists of an IPv4
 * address in dotted decimal format, optionally followed by a slash character
 * and a decimal prefix length.
 * Leading zeros are not permitted in the address or prefix length. This is
 * to avoid misinterpreting values which might have been intended as octal
 * (which is how inet_addr would interpret them).
 */
class inet4_netblock {
private:
	/** The IPv4 prefix. */
	unsigned char _prefix[4];

	/** The prefix length, in bits. */
	unsigned int _prefix_length;
public:
	/** Construct netblock from string.
	 * @param str the netblock as a string
	 */
	explicit inet4_netblock(const std::string& str);

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
