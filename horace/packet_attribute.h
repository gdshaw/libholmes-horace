// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_ATTRIBUTE
#define LIBHOLMES_HORACE_PACKET_ATTRIBUTE

#include "horace/packet_ref_attribute.h"

namespace horace {

/** A class to represent an owning packet content attribute.
 * This class differs from packet_ref_attribute in that it owns a copy
 * of the packet content, as opposed to referring to external content.
 */
class packet_attribute:
	public packet_ref_attribute {
public:
	/** Construct source attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	packet_attribute(octet_reader& in, size_t length);

	virtual ~packet_attribute();
};

} /* namespace horace */

#endif
