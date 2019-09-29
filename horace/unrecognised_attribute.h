// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNRECOGNISED_ATTRIBUTE
#define LIBHOLMES_HORACE_UNRECOGNISED_ATTRIBUTE

#include <memory>

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
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 */
	unrecognised_attribute(octet_reader& in, int type, size_t length);

	/** Construct unrecognised attribute.
	 * Ownership of the content passes to the attribute.
	 * @param type the required type
	 * @param content the required content
	 * @param length the required length, in octets
	 */
	unrecognised_attribute(int type, std::unique_ptr<const char[]>& content,
		size_t length);

	/** Get the content of this attribute.
	 * @return the content
	 */
	const char* content() const {
		return _content.get();
	}

	virtual size_t length() const {
		return _length;
	}

	virtual std::unique_ptr<attribute> clone() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
