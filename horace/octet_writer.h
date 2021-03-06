// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_OCTET_WRITER
#define LIBHOLMES_HORACE_OCTET_WRITER

#include <cstddef>
#include <cstring>
#include <string>

namespace horace {

/** An abstract base class for writing octets to an octet stream.
 * If any of the functions in this class cannot complete the requested
 * operation immediately then they will block until they are able to do so,
 * unless there is no prospect of that happening in which case a suitable
 * exception will be thrown.
 */
class octet_writer {
private:
	/** A pointer to the start of the buffer.
	 * It is permissible for _buffer, _limit and _ptr to be null if there
	 * is no buffer, but if one of these is null then all must be.
	 */
	char* _buffer;

	/** A pointer the first location beyond the end of the buffer. */
	char* _limit;

	/** A pointer to the first unoccupied location in the buffer. */
	char* _ptr;

	/** Write a given number of octets to the stream.
	 * This is a non-inline equivalent of the write function. It is
	 * expected that the write function will implement some of its
	 * functionality inline, then delegate the remainder to this
	 * function.
	 * @param buf the octets to be written
	 * @param nbyte the number of octets to write
	 */
	void _write(const void* buf, size_t nbyte);
protected:
	/** Write a given number of octets directly to the stream.
	 * This function is intended for internal use by the class
	 * implementation, either for flushing the buffer, or for
	 * performing direct writes which bypass the buffer.
	 *
	 * If it is not overridden, the default behaviour of this
	 * function is to throw an eof_error.
	 * @param buf the octets to be written
	 * @param nbyte the number of octets to write
	 */
	virtual void _write_direct(const void* buf, size_t nbyte);
public:
	/** Construct octet writer with no buffer. */
	octet_writer():
		_buffer(0),
		_limit(0),
		_ptr(0) {}

	/** Construct octet writer with given buffer.
	 * @param buffer the required buffer
	 * @param size the required buffer size, in octets
	 */
	octet_writer(void* buffer, size_t size):
		_buffer(static_cast<char*>(buffer)),
		_limit(_buffer + size),
		_ptr(_buffer) {}

	/** Destroy octet writer.
	 * Note that this base class destructor does not attempt to flush
	 * the buffer, on the grounds that it would be too late to write
	 * any data out by the time it is executed (since the vtable would
	 * have changed), however it may be appropriate for subclass
	 * destructors to flush any unwritten octets.
	 */
	virtual ~octet_writer() = default;

	octet_writer(const octet_writer&) = delete;
	octet_writer& operator=(const octet_writer&) = delete;

	/** Move-construct octet writer.
	 * @param that the octet writer to be moved
	 */
	octet_writer(octet_writer&& that):
		// Copy from old object to new.
		_buffer(that._buffer),
		_limit(that._limit),
		_ptr(that._ptr) {

		// Null the old object for safety.
		that._buffer = 0;
		that._limit = 0;
		that._ptr = 0;
	}

	/** Move-assign octet writer.
	 * @param that the octet writer to be moved
	 * @return a reference to this
	 */
	octet_writer& operator=(octet_writer&& that) {
		if (this != &that) {
			// Copy from old object to new.
			_buffer = that._buffer;
			_limit = that._limit;
			_ptr = that._ptr;

			// Null the old object for safety.
			that._buffer = 0;
			that._limit = 0;
			that._ptr = 0;
		}
		return *this;
	}

	/** Write a single octet to the stream.
	 * @param value the octet
	 */
	void write(char value) {
		if (_ptr == _limit) {
			flush();
		}
		*_ptr++ = value;
	}

	/** Write a given number of octets to the stream.
	 * @param buf the octets to be written
	 * @param nbyte the number of octets to write
	 */
	void write(const void* buf, size_t nbyte) {
		if (nbyte <= _limit - _ptr) {
			memcpy(_ptr, buf, nbyte);
			_ptr += nbyte;
		} else {
			_write(buf, nbyte);
		}
	}

	/** Flush any unwritten octets buffered by this class.
	 * If this function returns without throwing an exception
	 * then the buffer must be empty.
	 */
	void flush();

	/** Write an unsigned integer of given width to the stream.
	 * @param value the value to be written
	 * @param width the required width, in octets
	 */
	void write_unsigned(uint64_t value, size_t width);

	/** Write a signed integer of given width to the stream.
	 * @param value the value to be written
	 * @param width the required width, in octets
	 */
	void write_signed(int64_t value, size_t width);

	/** Write a character string to the stream.
	 * @param s the string to be written
	 */
	void write_string(const std::string& s);
};

} /* namespace horace */

#endif
