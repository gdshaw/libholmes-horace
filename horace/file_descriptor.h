// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_DESCRIPTOR
#define LIBHOLMES_HORACE_FILE_DESCRIPTOR

#include <cstddef>
#include <string>
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
		int mode = 0666);

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
		return *this;
	}

	/** Test whether this file descriptor is interruptible.
	 * @return true if interruptible, otherwise false
	 */
	bool interruptible() const;

	/** Set whether this file descriptor is interruptible.
	 * @param enable true if interruptible, otherwise false
	 */
	void interruptible(bool enable);

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
	 * This function will block until one of the specified event types
	 * is detected on the socket, or it is interrupted by a signal,
	 * or the optional timeout expires.
	 * @param events an event mask suitable for passing to poll
	 * @param timeout an optional timeout, in milliseconds
	 * @return the events which are ready
	 */
	int wait(int events, int timeout = -1) const;

	/** Check whether ready for activity.
	 * This function does not block.
	 * @param events an event mask suitable for passing to poll
	 * @return the events which are ready
	 */
	int ready(int events) const;

	/** Handle an revent of POLLERR.
	 * This function is needed in order to handle failed connection
	 * attempts using a non-blocking socket. It is virtual because the
	 * required action (involving a call to getsockopt) can only be
	 * applied to sockets and not to other types of file descriptor.
	 */
	virtual void handle_pollerror() const {}

	/** Read from file descriptor.
	 * If no octets are immediately available for reading then this
	 * function will block until either some have been read, or there
	 * is no further prospect of that happening. In the latter case
	 * a suitable exception will be thrown.
	 * @param buf a buffer to receive the result
	 * @param nbyte the maximum number of octets to be read
	 * @return the number of octets read
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

	/** Flush file content to durable storage. */
	void fsync() const;

	/** Try to establish an exclusive file lock.
	 * If a lock cannot be established immediately then this function
	 * will return without blocking.
	 * @return true if successful, otherwise false
	 */
	bool lock();

	/** Release exclusive file lock. */
	void unlock();
};

} /* namespace horace */

#endif
