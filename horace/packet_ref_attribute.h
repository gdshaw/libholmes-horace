// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_REF_ATTRIBUTE
#define LIBHOLMES_HORACE_PACKET_REF_ATTRIBUTE

#include "horace/attribute.h"

namespace horace {

/** A class to represent a non-owning packet content attribute.
 * This class allows an attribute of type ATTR_PACKET to refer to
 * pre-existing content without making a copy of it.
 */
class packet_ref_attribute:
	public attribute {
private:
	/** The content of the packet. */
	const char* _content;

	/** The length of the packet, in octets. */
	size_t _length;
public:
	/** Construct packet attribute.
	 * The content is not copied, and must remain valid for the
	 * lifetime of this attribute and of any record buffer derived
	 * from it.
	 * @param content the content of the packet
	 * @param length the length of the content, in octets
	 */
	packet_ref_attribute(const void* content, size_t length);

	/** Get content.
	 * @return the content of the packet
	 */
	const void* content() const {
		return _content;
	}

	virtual int type() const {
		return ATTR_PACKET;
	}

	/** Get length.
	 * @return the length of the packet.
	 */
	size_t length() const {
		return _length;
	}

	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
