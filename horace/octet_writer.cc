// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/eof_error.h"
#include "horace/octet_writer.h"

namespace horace {

void octet_writer::_write(const void* buf, size_t nbyte) {
	if (nbyte <= _limit - _ptr) {
		// This condition should normally have been handled by
		// the inline function, but repeated here in case this
		// function is called directly.
		memcpy(_ptr, buf, nbyte);
		_ptr += nbyte;
	} else {
		// First flush the buffer if it is non-empty.
		size_t bcount = _ptr - _buffer;
		if (bcount) {
			_write_direct(_buffer, bcount);
			_ptr = _buffer;
		}

		// Now either copy the new data into the buffer, or if
		// it would fill more than half of the buffer, writer it
		// directly.
		if (nbyte * 2 >= _limit - _ptr) {
			_write_direct(buf, nbyte);
		} else {
			memcpy(_ptr, buf, nbyte);
			_ptr += nbyte;
		}
	}
}

void octet_writer::_write_direct(const void* buf, size_t nbyte) {
	throw eof_error();
}

void octet_writer::flush() {
	size_t bcount = _ptr - _buffer;
	if (bcount) {
		_write_direct(_buffer, bcount);
		_ptr = _buffer;
	}
}

void octet_writer::write_unsigned(uint64_t value, size_t width) {
	for (size_t i = 0, j = (width - 1) * 8; i != width; i += 1, j -= 8) {
		write((value >> j) & 0xff);
	}
}

void octet_writer::write_base128(uint64_t value) {
	int size = 7;
	while ((value >> size) != 0) {
		size += 7;
	}
	while (size > 7) {
		size -= 7;
		write(0x80 | ((value >> size) & 0x7f));
        }
	write(value & 0x7f);
}

void octet_writer::write_string(const std::string& s) {
	write(s.data(), s.length());
}

} /* namespace horace */
