// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_OCTET_READER
#define LIBHOLMES_HORACE_OCTET_READER

#include <cstddef>
#include <cstring>
#include <string>

namespace horace {

/** An abstract base class for reading octets from an octet stream.
 * Unless otherwise specified, if any of the functions in this class cannot
 * complete the requested operation immediately then they will block until
 * they are able to do so, unless there is no prospect of that happening
 * in which case a suitable exception will be thrown.
 */
class octet_reader {
private:
	/** A pointer to the start of the buffer.
	 * It is permissible for _buffer, _limit, _ptr and _end to be null if
	 * there is no buffer, but if one of these is null then all must be.
	 */
	char* _buffer;

	/** A pointer the first location beyond the end of the buffer. */
	char* _limit;

	/** A pointer to the first unread octet in the buffer, or equal to
	 * _end if there are no unread octets. */
	char* _ptr;

	/** A pointer to the first location beyond the end of the occupied
	 * region of the buffer. */
	char* _end;

	/** Refill the buffer. */
	void _refill();

	/** Read a given number of octets from the stream.
	 * The required behaviour for this function is the same as for the
	 * read function. It is intended as a fallback in the case where the
	 * number of octets immediately available in the buffer is
	 * insufficient to satisfy the request. This allows the relevant
	 * functionality to be split between inline and non-inline code.
	 * @param buf a buffer to receive the octets
	 * @param nbyte the number of octets to write
	 */
	void _read(void* buf, size_t nbyte);
protected:
	/** Read up to a given number of octets directly to the stream.
	 * This function is intended for internal use by the class
	 * implementation, either for refilling the buffer, or for
	 * performing direct reads which bypass the buffer.
	 *
	 * If it is not overridden, the default behaviour of this
	 * function is to throw an eof_error.
	 *
	 * If there are no octets immediately available for reading then
	 * this function will block until either some octets have been read,
	 * or there is no further prospect of reading any.
	 * @param buf a buffer to receive the octets
	 * @param nbyte the number of octets to write
	 * @return the number of octets read
	 */
	virtual size_t _read_direct(void* buf, size_t nbyte);
public:
	/** Construct octet reader with no buffer. */
	octet_reader():
		_buffer(0),
		_limit(0),
		_ptr(0),
		_end(0) {}

	/** Construct octet reader with given buffer.
	 * @param buffer the buffer
	 * @param size the buffer size
	 * @param count the number of octets initially available for reading
	 */
	octet_reader(void* buffer, size_t size, size_t count):
		_buffer(static_cast<char*>(buffer)),
		_limit(_buffer + size),
		_ptr(_buffer),
		_end(_buffer + count) {}

	/** Destroy octet reader. */
	virtual ~octet_reader() = default;

	octet_reader(const octet_reader&) = delete;
	octet_reader& operator=(const octet_reader&) = delete;

	/** Move-construct octet reader.
	 * @param that the octet reader to be moved
	 */
	octet_reader(octet_reader&& that):
		// Copy from old object to new.
		_buffer(that._buffer),
		_limit(that._limit),
		_ptr(that._ptr),
		_end(that._end) {

		// Null the old object for safety.
		that._buffer = 0;
		that._limit = 0;
		that._ptr = 0;
		that._end = 0;
	}

	/** Move-assign octet reader.
	 * @param that the octet reader to be moved
	 * @return a reference to this
	 */
	octet_reader& operator=(octet_reader&& that) {
		if (this != &that) {
			// Copy from old object to new.
			_buffer = that._buffer;
			_limit = that._limit;
			_ptr = that._ptr;
			_end = that._end;

			// Null the old object for safety.
			that._buffer = 0;
			that._limit = 0;
			that._ptr = 0;
			that._end = 0;
		}
		return *this;
	}

	/** Read a single octet from the stream.
	 * @return the octet
	 */
	char read() {
		if (_ptr == _end) {
			_refill();
		}
		return *_ptr++;
	}

	/** Read a given number of octets from the stream.
	 * @param buf a buffer to receive the octets
	 * @param nbyte the number of octets to write
	 */
	void read(void* buf, size_t nbyte) {
		if (nbyte <= _end - _ptr) {
			memcpy(buf, _ptr, nbyte);
			_ptr += nbyte;
		} else {
			_read(buf, nbyte);
		}
	}

	/** Read an unsigned integer of given width from the stream.
	 * @param width the required width, in octets
	 * @return the decoded value of the integer
	 */
	uint64_t read_unsigned(size_t width);

	/** Read an unsigned base-128 integer from the stream.
	 * @return the decoded value of the integer
	 */
	uint64_t read_unsigned_base128();

	/** Read an unsigned base-128 integer from the stream.
	 * @param count a counter for the number of octets read
	 * @return the decoded value of the integer
	 */
	uint64_t read_unsigned_base128(size_t& count);

	/** Read a signed base-128 integer from the stream.
	 * @return the decoded value of the integer
	 */
	int64_t read_signed_base128();

	/** Read a signed base-128 integer from the stream.
	 * @param count a counter for the number of octets read
	 * @return the decoded value of the integer
	 */
	int64_t read_signed_base128(size_t& count);

	/** Read a character string from the stream.
	 * @param length the required length, in octets
	 * @return the resulting character string
	 */
	std::string read_string(size_t length);
};

} /* namespace horace */

#endif
