// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_LENGTH_ATTRIBUTE
#define LIBHOLMES_HORACE_PACKET_LENGTH_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** An attribute class to represent the original length of a packet.
 * Attributes of this type are intended for use when a captured network
 * packet has been truncated, and therefore its original length differs
 * from the length of the captured data.
 *
 * Use of this attribute in the absence of truncation is not forbidden,
 * however it serves no useful purpose since the default assumption is
 * that the two lengths are equal.
 */
class packet_length_attribute:
	public attribute {
private:
	/** The original length of the packet, in octets. */
	uint64_t _origlen;
public:
	/** Construct packet length attribute from length
	 * @param origlen the original length of the packet, in octets
	 */
	explicit packet_length_attribute(uint64_t origlen);

	/** Construct packet length attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	packet_length_attribute(octet_reader& in, size_t length);

	/** Get the original packet length.
	 * This should not be confused with the length function, which
	 * returns the encoded length of the attribute content (as opposed
	 * to the length of the network packet).
	 * @return original length of the packet, in octets
	 */
	uint64_t origlen() const {
		return _origlen;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
