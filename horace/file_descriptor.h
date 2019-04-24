// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_DESCRIPTOR
#define LIBHOLMES_HORACE_FILE_DESCRIPTOR

#include <cstddef>
#include <utility>

namespace horace {

/** A class to represent a file descriptor.
 * File descriptors are movable but not copyable.
 */
class file_descriptor {
private:
	/** The raw file descriptor. */
	int _fd;
public:
	/** Create empty file descriptor object. */
	file_descriptor():
		_fd(-1) {}

	/** Wrap raw file descriptor.
	 * The resulting class instance takes ownership of the raw file
	 * descriptor passed to it.
	 *
	 * If a value of -1 is passed in then this constructor will not
	 * overwrite the value of errno.
	 * @param fd the raw file descriptor, or -1 for an empty file
	 *  descriptor object
	 */
	explicit file_descriptor(int fd):
		_fd(fd) {}

	/** Open file.
	 * The resulting file descriptor object will be non-empty, or if
	 * that is not possible an exception will be thrown.
	 * @param pathname the file pathname
	 * @param flags flags suitable for passing to open
	 * @param mode the required mode if new file created
	 */
	file_descriptor(const std::string& pathname, int flags,
		int mode = 0644);

	/** Close file descriptor. */
	virtual ~file_descriptor();

	file_descriptor(const file_descriptor&) = delete;
	file_descriptor& operator=(const file_descriptor&) = delete;

	/** Move-construct file descriptor.
	 * @param that the file descriptor to be moved
	 */
	file_descriptor(file_descriptor&& that) {
		this->_fd = that._fd;
		that._fd = -1;
	}

	/** Move-assign file descriptor.
	 * @param that the file descriptor to be moved
	 */
	file_descriptor& operator=(file_descriptor&& that) {
		std::swap(this->_fd, that._fd);
	}

	/** Test whether file descriptor object is non-empty.
	 * @return true if non-empty, otherwise false
	 */
	operator bool() const {
		return _fd != -1;
	}

	/** Get raw file descriptor.
	 * @return the raw file descriptor, or -1 if empty
	 */
	operator int() const {
		return _fd;
	}

	/** Block awaiting activity.
	 * This function will block until either one of the specified event
	 * types is detected on the socket, or it is interrupted by a signal.
	 * @param events an event mask suitable for passing to ppoll
	 */
	void wait(int events) const;

	/** Read from file descriptor.
	 * If no octets are immediately available for reading then this
	 * function will block until either some have been read, or there
	 * is no further prospect of that happening. In the latter case
	 * a suitable exception will be thrown.
	 * @param buf a buffer to receive the result
	 * @param nbyte the maximum number of octets to be read
	 */
	size_t read(void* buf, size_t nbyte);

	/** Write to file descriptor.
	 * If the requested number of octets cannot be written immediately
	 * then this function will block until either that has been done,
	 * or there is no further prospect of that happening. In the latter
	 * case a suitable exception will be thrown.
	 * @param buf the data to be written
	 * @param nbyte the number of octets to be written
	 */
	void write(const void* buf, size_t nbyte);
};

} /* namespace horace */

#endif
