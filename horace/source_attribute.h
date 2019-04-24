// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SOURCE_ATTRIBUTE
#define LIBHOLMES_HORACE_SOURCE_ATTRIBUTE

#include <string>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a source attribute.
 * A source attribute contains a source identifier, which is used to link
 * together sessions which form part of the same time series.
 */
class source_attribute:
	public attribute {
private:
	/** The source identifier. */
	std::string _source_id;
public:
	/** Construct source attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	source_attribute(octet_reader& in, size_t length);

	/** Construct source attribute.
	 * @param source_id the source identifier
	 */
	source_attribute(const std::string& source_id);

	/** Get the source identifier.
	 * @return the source identifier
	 */
	const std::string& source_id() const {
		return _source_id;
	}

	virtual int type() const;
	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
