// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_COMPOUND_ATTRIBUTE
#define LIBHOLMES_HORACE_COMPOUND_ATTRIBUTE

#include "horace/attribute.h"
#include "horace/attribute_list.h"

namespace horace {

/** A class to represent an attribute containing a list of sub-attributes. */
class compound_attribute:
	public attribute {
private:
	/** The list of sub-attributes. */
	attribute_list _attrlist;
public:
	/** Construct compound attribute.
	 * @param type the attribute type
	 * @param attrlist the required attributes
	 */
	compound_attribute(int type, attribute_list&& attrlist);

	/** Construct compound attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	compound_attribute(int type, size_t length, octet_reader& in);

	/** Get the attribute content.
	 * @return the content
	 */
	const attribute_list& content() const {
		return _attrlist;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
