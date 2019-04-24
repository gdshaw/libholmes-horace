// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SEQNUM_ATTRIBUTE
#define LIBHOLMES_HORACE_SEQNUM_ATTRIBUTE

#include <cstdint>

#include "horace/attribute.h"

namespace horace {

/** A class to represent a sequence number attribute. */
class seqnum_attribute:
	public attribute {
private:
	/** The sequence number. */
	uint64_t _seqnum;
public:
	/** Construct sequence number attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	seqnum_attribute(octet_reader& in, size_t length);

	/** Construct sequence number attribute.
	 * @param seqnum the sequence number
	 */
	seqnum_attribute(uint64_t seqnum);

	/** Get the sequence number.
	 * @return the sequence number
	 */
	uint64_t seqnum() const {
		return _seqnum;
	}

	virtual int type() const {
		return ATTR_SEQNUM;
	}

	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
