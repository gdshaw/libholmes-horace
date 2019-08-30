// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LINKTYPE_ATTRIBUTE
#define LIBHOLMES_HORACE_LINKTYPE_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a link-layer header type.
 * This is the applicable LINKTYPE_ constant as defined by libpcap
 * (http://www.tcpdump.org/linktypes.html).
 */
class linktype_attribute:
	public attribute {
private:
	/** The linktype. */
	uint64_t _linktype;
public:
	/** Construct linktype attribute.
	 * @param linktype the linktype
	 */
	explicit linktype_attribute(uint64_t linktype);

	/** Construct linktype attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	linktype_attribute(octet_reader& in, size_t length);

	/** Get the linktype.
	 * @return the linktype
	 */
	uint64_t linktype() const {
		return _linktype;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
