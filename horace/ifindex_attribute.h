// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_IFINDEX_ATTRIBUTE
#define LIBHOLMES_HORACE_IFINDEX_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a network interface index attribute. */
class ifindex_attribute:
	public attribute {
private:
	/** The network interface index. */
	uint64_t _ifindex;
public:
	/** Construct interface index attribute.
	 * @param seqnum the interface index
	 */
	explicit ifindex_attribute(uint64_t ifindex);

	/** Construct interface index attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	ifindex_attribute(octet_reader& in, size_t length);

	/** Get the interface index.
	 * @return the interface index
	 */
	uint64_t ifindex() const {
		return _ifindex;
	}

	virtual int type() const {
		return ATTR_IFINDEX;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
