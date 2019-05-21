// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_REPEAT_ATTRIBUTE
#define LIBHOLMES_HORACE_REPEAT_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** An attribute class to indicate that an event occurred repeatedly. */
class repeat_attribute:
	public attribute {
private:
	/** The number of repetitions. */
	uint64_t _count;
public:
	/** Construct repeat attribute.
	 * @param count the number of repetitions
	 */
	explicit repeat_attribute(uint64_t count);

	/** Construct repeat attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	repeat_attribute(octet_reader& in, size_t length);

	/** Get the number of repetitions
	 * @return the number of repetitions
	 */
	uint64_t count() const {
		return _count;
	}

	virtual int type() const {
		return ATTR_REPEAT;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
