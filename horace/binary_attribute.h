// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_BINARY_ATTRIBUTE
#define LIBHOLMES_HORACE_BINARY_ATTRIBUTE

#include "horace/binary_ref_attribute.h"

namespace horace {

/** A class to represent an attribute owning a binary string.
 * This class differs from binary_ref_attribute in that it owns a copy
 * of the content, as opposed to referring to external content.
 */
class binary_attribute:
	public binary_ref_attribute {
public:
	/** Construct binary attribute.
	 * The supplied content is copied, allowing the original to be
	 * discarded once the attribute has been constructed.
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 * @param content the required content
	 */
	binary_attribute(int type, size_t length, const void* content);

	/** Construct binary attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	binary_attribute(int type, size_t length, octet_reader& in);

	virtual ~binary_attribute();
};

} /* namespace horace */

#endif
