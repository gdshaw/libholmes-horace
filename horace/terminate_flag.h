// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TERMINATE_FLAG
#define LIBHOLMES_HORACE_TERMINATE_FLAG

#include <atomic>

#include "horace/terminate_exception.h"

namespace horace {

/** A class for indicating that the process is terminating.
 * The member functions in this class are thread-safe.
 */
class terminate_flag {
private:
	/** True if process terminating, otherwise false. */
	std::atomic_bool _terminating;

	/** File descriptors for a self-pipe.
	 * This is in the format passed to pipe(2), so the first
	 * file descriptor is for reading and the second one for
	 * writing.
	 */
	int _pipefd[2];

	/** Mark a given file descriptor as non-blocking.
	 * @param fd the file descriptor
	 */
	static void _nonblock(int fd);
public:
	/** Construct termination flag. */
	terminate_flag();

	/** Set or clear the termination flag.
	 * @param terminating true if terminating, false if not
	 */
	terminate_flag& operator=(bool terminating);

	/** Poll termination flag.
	 * Throw a terminate_exception if the flag is set.
	 */
	void poll() {
		if (_terminating) {
			throw terminate_exception();
		}
	}

	/** Poll a given file descriptor, with optional timeout.
	 * @param fd the file descriptor on which to poll
	 * @param events the required events mask for fd
	 * @param timeout the optional timeout, in milliseconds
	 * @return the resulting revents mask for rd
	 */
	int poll(int fd, int events, int timeout = -1) const;

	/** Sleep for a given number of milliseconds.
	 * Note that the current implementation does not resume sleeping
	 * if interrupted by a signal.
	 * @param timeout the time for which to sleep, in milliseconds
	 */
	void millisleep(int timeout) const;
};

/** A flag to indicate that the process is terminating. */
extern terminate_flag terminating;

} /* namespace horace */

#endif
