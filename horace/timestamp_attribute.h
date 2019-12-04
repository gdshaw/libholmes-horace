// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TIMESTAMP_ATTRIBUTE
#define LIBHOLMES_HORACE_TIMESTAMP_ATTRIBUTE

#include <time.h>

#include "horace/attribute.h"

namespace horace {

/** A class to represent an attribute containing a timestamp.
 * A timestamp consists of a number of whole seconds and a number of
 * nanoseconds. It is interpreted with respect to a time system which is
 * applicable to the session as a whole (and therefore not explicitly
 * specified as part of the timestamp).
 *
 * The number of nanoseconds must be in the range 0 to 999999999
 * inclusive, excepting that the range 1000000000 to 1999999999 may be
 * used to represent a leap second if permitted by the time system.
 */
class timestamp_attribute:
	public attribute {
private:
	/** The attribute content.
	 * Note that, depending on the applicable time system, this will
	 * not necessarily have the sematics normally associated with a
	 * POSIX struct timespec.
	 */
	struct timespec _content;
public:
	/** Construct timestamp attribute from an octet reader.
	 * The ID and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param attrid the attribute ID
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	timestamp_attribute(int attrid, size_t length, octet_reader& in);

	/** Construct timestamp attribute for current time.
	 * @param attrid the attribute ID
	 */
	explicit timestamp_attribute(int addrid);

	/** Construct timestamp attribute from seconds and nanoseconds.
	 * @param attrid the attribute ID
	 * @param sec the number of whole seconds since the epoch
	 * @param nsec the number of additional nanoseconds
	 */
	timestamp_attribute(int attrid, time_t sec, long nsec);

	/** Construct timestamp attribute from a timespec.
	 * @param attrid the attribute ID
	 * @param ts the required timestamp
	 */
	timestamp_attribute(int attrid, const struct timespec& ts);

	/** Get the attribute content.
	 * @return the content as a timespec
	 */
	const struct timespec& content() const {
		return _content;
	}

	virtual size_t length() const;
	virtual std::unique_ptr<attribute> clone() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
