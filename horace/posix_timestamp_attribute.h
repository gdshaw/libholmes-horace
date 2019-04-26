// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_POSIX_TIMESTAMP_ATTRIBUTE
#define LIBHOLMES_HORACE_POSIX_TIMESTAMP_ATTRIBUTE

#include <sys/types.h>

#include "horace/absolute_timestamp_attribute.h"

namespace horace {

/** A class to represent a POSIX timestamp attribute.
 * This type of timestamp attribute specifies the number of whole seconds
 * since the POSIX epoch 1970-01-01 00:00:00 UTC, excluding leap seconds.
 */
class posix_timestamp_attribute:
	public absolute_timestamp_attribute {
private:
	/** The timestamp as a time_t. */
	time_t _ts;
public:
	/** Construct POSIX timestamp attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	posix_timestamp_attribute(octet_reader& in, size_t length);

	/** Construct POSIX timestamp attribute.
	 * @param ts the required timestamp as a time_t
	 */
	posix_timestamp_attribute(time_t ts);

	/** Get the timestamp.
	 * @return the timestamp as a time_t
	 */
	time_t timestamp() const {
		return _ts;
	}

	virtual int type() const {
		return ATTR_POSIX_TIMESTAMP;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif