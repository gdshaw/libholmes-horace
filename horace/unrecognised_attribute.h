// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNRECOGNISED_ATTRIBUTE
#define LIBHOLMES_HORACE_UNRECOGNISED_ATTRIBUTE

#include "horace/attribute.h"

namespace horace {

/** A class to represent an attribute of arbitrary type. */
class unrecognised_attribute:
	public attribute {
private:
	/** The length of the raw content, in octets. */
	size_t _length;

	/** The raw content of this attribute. */
	std::unique_ptr<const char[]> _content;
public:
	/** Construct unrecognised attribute from an octet reader.
	 * The ID and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param attrid the attribute ID
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	unrecognised_attribute(int attrid, size_t length, octet_reader& in);

	/** Construct unrecognised attribute.
	 * Ownership of the content passes to the attribute.
	 * @param attrid the required ID
	 * @param content the required content
	 * @param length the required length, in octets
	 */
	unrecognised_attribute(int attrid, size_t length,
		std::unique_ptr<const char[]>& content);

	/** Get the content of this attribute.
	 * @return the content
	 */
	const char* content() const {
		return _content.get();
	}

	virtual size_t length() const;
	virtual std::unique_ptr<attribute> clone() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
