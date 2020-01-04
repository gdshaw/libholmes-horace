// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNSIGNED_BASE128_INTEGER
#define LIBHOLMES_HORACE_UNSIGNED_BASE128_INTEGER

#include <cstdint>

namespace horace {

class octet_reader;
class octet_writer;

/** A class for encoding or decoding unsigned base-128 integers. */
class unsigned_base128_integer {
private:
	/** The unencoded value of the integer. */
	uint64_t _value;

	/** The encoded length, in octets. */
	size_t _length;
public:
	/** Construct instance from unencoded value.
	 * @param value the value to be encoded
	 */
	unsigned_base128_integer(uint64_t value);

	/** Construct instance by reading encoded value from octet reader.
	 * @param reader the octet reader
	 */
	unsigned_base128_integer(octet_reader& reader);

	/** Get the unencoded value.
	 * @return the unencoded value
	 */
	operator uint64_t() const {
		return _value;
	}

	/** Get the encoded length.
	 * @return the length, in octets
	 */
	size_t length() const {
		return _length;
	}

	/** Write the encoded value to an octet writer.
	 * @param writer the octet writer
	 */
	void write(octet_writer& writer) const;
};

} /* namespace horace */

#endif
