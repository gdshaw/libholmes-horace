// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RELATIVE_TIMESTAMP_ATTRIBUTE
#define LIBHOLMES_HORACE_RELATIVE_TIMESTAMP_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a relative timestamp attribute.
 * The unit of measure is implied by the attribute type. The point of
 * reference is the session start timestamp (which cannot itself be a
 * relative timestamp).
 */
class relative_timestamp_attribute:
	public attribute {
private:
	/** The timestamp as a number of time units. */
	uint64_t _ts;

	/** The attribute type. */
	int _type;
public:
	/** Construct relative timestamp attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 */
	relative_timestamp_attribute(octet_reader& in, int type, size_t length);

	/** Construct relative timestamp attribute.
	 * @param ts the timestamp as a number of time units
	 * @param type the attribute type
	 */
	relative_timestamp_attribute(uint64_t ts, int type);

	/** Get the timestamp.
	 * @return the timestamp as a number of time units
	 */
	uint64_t timestamp() const {
		return _ts;
	}

	/** Get the unit of measurement as a divisor.
	 * @return the number of units per second
	 */
	uint64_t unit_divisor() const;

	/** Get the unit of measurement as a string.
	 * @return the unit of measurement
	 */
	std::string unit_name() const;

	virtual int type() const {
		return _type;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
