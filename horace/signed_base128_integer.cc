// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <limits>
#include <stdexcept>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/signed_base128_integer.h"

namespace horace {

signed_base128_integer::signed_base128_integer(int64_t value):
	_tcvalue(value),
	_length(1) {

	// Get the set of bits which differ from the sign bit.
	// Any leading zeros in this value need not be encoded.
	uint64_t dvalue = (value >= 0) ? _tcvalue : ~_tcvalue;

	// There must be at least 1 octet, which encodes the sign bit and
	// 6 other bits. Each additional octet encodes 7 further bits.
	unsigned int size = 6;
	while ((dvalue >> size) != 0) {
		size += 7;
		_length += 1;
	}
}

signed_base128_integer::signed_base128_integer(octet_reader& reader):
	_length(1) {

	// There must always be at least one octet to decode, with the sign
	// in bit 6 and content in its least significant 6 bits.
	uint8_t byte = reader.read();
	_tcvalue = byte & 0x3f;
	_tcvalue -= byte & 0x40;

	// The initial octet might also be the final octet, but if not
	// then continue decoding until the final octet has been decoded.
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 8 bits are either all ones or all zeros.
		if (((_tcvalue >> 56) + 1) & 0xfe) {
			throw std::runtime_error("integer overflow");
		}
		_tcvalue <<= 7;
		byte = reader.read();
		_length += 1;
		_tcvalue |= byte & 0x7f;
	}
}

signed_base128_integer::operator int64_t() const {
	// Mask out the sign bit from _tcvalue.
	int64_t value = _tcvalue & std::numeric_limits<int64_t>::max();

	// If the sign bit is clear then that is the result.
	// Otherwise, add -2^63 to obtain the result.
	if (_tcvalue & std::numeric_limits<int64_t>::min()) {
		value += std::numeric_limits<int64_t>::min();
	}
	return value;
}

void signed_base128_integer::write(octet_writer& writer) const {
	// Each octet encodes 7 bits, but one of these in the first octet
	// is needed for the sign bit.
	unsigned int size = _length * 7;

	// It is possible for there to be 10 octets, but if there are then
	// the first one only encodes the sign bit. The continuation bit
	// should be set, and the other bits should match the sign bit,
	// therefore the only possible values are 0x80 and 0xff.
	while (size > 63) {
		size -= 7;
		if (_tcvalue & std::numeric_limits<int64_t>::min()) {
			writer.write(0xff);
		} else {
			writer.write(0x80);
		}
	}

	// Encode any further non-final octets, with the continuation
	// bit set.
	while (size > 7) {
		size -= 7;
		writer.write(0x80 | ((_tcvalue >> size) & 0x7f));
	}

	// Encode the final octet, with the continuation bit clear.
	writer.write(_tcvalue & 0x7f);
}

} /* namespace horace */
