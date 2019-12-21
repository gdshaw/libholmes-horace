// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNSIGNED_INTEGER_ATTRIBUTE
#define LIBHOLMES_HORACE_UNSIGNED_INTEGER_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** A class to represent an attribute containing an unsigned integer. */
class unsigned_integer_attribute:
	public attribute {
private:
	/** The attribute content. */
	uint64_t _content;
public:
	/** Construct unsigned integer attribute.
	 * @param attrid the required attribute ID
	 * @param content the required content
	 */
	unsigned_integer_attribute(int attrid, uint64_t content);

	/** Construct unsigned integer attribute from an octet reader.
	 * The ID and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param attrid the attribute ID
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	unsigned_integer_attribute(int attrid, size_t length, octet_reader& in);

	virtual bool operator==(const attribute& that) const;

	/** Get the attribute content.
	 * @return the content
	 */
	uint64_t content() const {
		return _content;
	}

	virtual size_t length() const;
	virtual std::unique_ptr<attribute> clone() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
