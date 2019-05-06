// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_BUFFER
#define LIBHOLMES_HORACE_BUFFER

#include <memory>

namespace horace {

/** A base class for providing a buffer as part of another class.
 * This is implemented as a base class so that the buffer can be used in
 * the constructor for another base class.
 */
class buffer {
private:
	/** The buffer. */
	std::unique_ptr<char[]> _buffer;

	/** The size of the buffer. */
	size_t _size;
public:
	/** Construct null buffer. */
	buffer() {}

	/** Construct buffer.
	 * @param size the required size of the buffer, in octets
	 */
	explicit buffer(size_t size):
		_buffer(std::make_unique<char[]>(size)),
		_size(size) {}

	/** Get a pointer to the buffer.
	 * @return a pointer to the buffer
	 */
	char* buffer_ptr() const {
		return _buffer.get();
	}

	/** Get the size of the buffer.
	 * @return the size of the buffer, in octets
	 */
	size_t buffer_size() const {
		return _size;
	}
};

} /* namespace horace */

#endif
