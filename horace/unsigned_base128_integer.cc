// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_base128_integer.h"

namespace horace {

unsigned_base128_integer::unsigned_base128_integer(uint64_t value):
	_value(value),
	_length(1) {

	// One encoded octet for every 7 unencoded bits, but with a
	// minimum length of 1 octet.
	unsigned int size = 7;
	while ((_value >> size) != 0) {
		size += 7;
		_length += 1;
	}
}

unsigned_base128_integer::unsigned_base128_integer(octet_reader& reader):
	_length(1) {

	// There must always be at least one octet to decode, with content
	// in its least significant 7 bits.
	uint8_t byte = reader.read();
	_value = byte & 0x7f;

	// The initial octet might also be the final octet, but if not
	// then continue decoding until the final octet has been decoded.
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 7 bits are all zeros.
		if (_value >> 57) {
			throw std::runtime_error("integer overflow");
		}
		_value <<= 7;
		byte = reader.read();
		_length += 1;
		_value |= byte & 0x7f;
	}
}

void unsigned_base128_integer::write(octet_writer& writer) const {
	// Each octet encodes 7 bits.
	unsigned int size = _length * 7;

	// Encode any non-final octets, with the continuation bit set.
	while (size > 7) {
		size -= 7;
		writer.write(0x80 | ((_value >> size) & 0x7f));
	}

	// Encode the final octet, with the continuation bit clear.
	writer.write(_value & 0x7f);
}

} /* namespace horace */
