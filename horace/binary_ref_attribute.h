// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_BINARY_REF_ATTRIBUTE
#define LIBHOLMES_HORACE_BINARY_REF_ATTRIBUTE

#include "horace/attribute.h"

namespace horace {

/** A class to represent an attribute referring to a binary string.
 * This class allows an attribute to refer to a pre-existing binary
 * string without making a copy of it.
 */
class binary_ref_attribute:
	public attribute {
private:
	/** The attribute content. */
	const char* _content;

	/** The length of the attribute content, in octets. */
	size_t _length;
public:
	/** Construct binary attribute.
	 * The content is not copied, and must remain valid for the
	 * lifetime of this attribute and of any record buffer derived
	 * from it.
	 * @param attrid the attribute ID
	 * @param length the length of the content, in octets
	 * @param content the required content
	 */
	binary_ref_attribute(int attrid, size_t length, const void* content);

	/** Get attribute content.
	 * @return the content
	 */
	const void* content() const {
		return _content;
	}

	virtual size_t length() const;
	virtual std::unique_ptr<attribute> clone() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
