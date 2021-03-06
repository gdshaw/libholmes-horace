// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <memory>

#include "horace/eof_error.h"
#include "horace/octet_reader.h"

namespace horace {

void octet_reader::_refill() {
	size_t rcount = _read_direct(_buffer, _limit - _buffer);
	if (!rcount) {
		throw eof_error();
	}
	_ptr = _buffer;
	_end = _buffer + rcount;
}

void octet_reader::_read(void* buf, size_t nbyte) {
	char* bptr = static_cast<char*>(buf);
	while (nbyte) {
		size_t bcount = _end - _ptr;
		if (bcount) {
			// If there is data in the buffer then use that
			// first.
			if (bcount > nbyte) {
				bcount = nbyte;
			}
			memcpy(bptr, _ptr, bcount);
			bptr += bcount;
			_ptr += bcount;
			nbyte -= bcount;
		} else if (nbyte * 2 >= _limit - _buffer) {
			// If there is half a buffer-full or more remaining
			// to be read then attempt to do so directly.
			size_t rcount = _read_direct(bptr, nbyte);
			if (!rcount) {
				throw eof_error();
			}
			bptr += rcount;
			nbyte -= rcount;
		} else {
			// Otherwise, attempt to refill the buffer.
			size_t rcount = _read_direct(_buffer,
				_limit - _buffer);
			if (!rcount) {
				throw eof_error();
			}
			_ptr = _buffer;
			_end = _buffer + rcount;
		}
	}
}

size_t octet_reader::_read_direct(void* buf, size_t nbyte) {
	throw eof_error();
}

uint64_t octet_reader::read_unsigned(size_t width) {
	uint64_t result = 0;
	while (width--) {
		// The result can be shifted without overflow if the top
		// 8 bits are all zeros.
		if (result >> 56) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 8;
		result |= read() & 0xff;
	}
	return result;
}

int64_t octet_reader::read_signed(size_t width) {
	int64_t result = 0;
	if (width) {
		uint8_t byte = read();
		result = (byte & 0x7f) - (byte & 0x80);
		width -= 1;
	}
	while (width--) {
		// The result can be shifted without overflow if the top
		// 9 bits are either all ones or all zeros.
		if (((result >> 55) + 1) >> 1) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 8;
		result |= read() & 0xff;
	}
	return result;
}

uint64_t octet_reader::read_unsigned_base128() {
	uint8_t byte = read();
	uint64_t result = byte & 0x7f;
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 7 bits are all zeros.
		if (result >> 57) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 7;
		byte = read();
		result |= byte & 0x7f;
	}
	return result;
}

uint64_t octet_reader::read_unsigned_base128(size_t& count) {
	uint8_t byte = read();
	count += 1;
	uint64_t result = byte & 0x7f;
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 7 bits are all zeros.
		if (result >> 57) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 7;
		byte = read();
		count += 1;
		result |= byte & 0x7f;
	}
	return result;
}

int64_t octet_reader::read_signed_base128() {
	uint8_t byte = read();
	int64_t result = (byte & 0x3f) - (byte & 0x40);
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 8 bits are either all ones or all zeros.
		if (((result >> 56) + 1) >> 1) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 7;
		byte = read();
		result |= byte & 0x7f;
	}
	return result;
}

int64_t octet_reader::read_signed_base128(size_t& count) {
	uint8_t byte = read();
	count += 1;
	int64_t result = (byte & 0x3f) - (byte & 0x40);
	while (byte & 0x80) {
		// The result can be shifted without overflow if the top
		// 8 bits are either all ones or all zeros.
		if (((result >> 56) + 1) >> 1) {
			throw std::runtime_error("integer overflow");
		}
		result <<= 7;
		byte = read();
		count += 1;
		result |= byte & 0x7f;
	}
	return result;
}

std::string octet_reader::read_string(size_t length) {
	std::unique_ptr<char[]> buffer = std::make_unique<char[]>(length);
	read(buffer.get(), length);
	return std::string(buffer.get(), length);
}

} /* namespace horace */
