// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LIBC_ERROR
#define LIBHOLMES_HORACE_LIBC_ERROR

#include <stdexcept>

namespace horace {

/** A helper class for holding the value of errno. */
class libc_error_holder {
private:
	/** The error number. */
	int _errno;
public:
	/** Construct errno holder for a specific error number.
	 * @param errno_arg the required error number
	 */
	explicit libc_error_holder(int errno_arg):
		_errno(errno_arg) {}

	/** Get the error number.
	 * @return the error number
	 */
	int errno_value() const {
		return _errno;
	}

	/** Get a string corresponding to errno.
	 * @return the string
	 */
	const char* strerror() const;
};

/** A class to represent an error corresponding to a standard C library
 * error number. */
class libc_error:
	public libc_error_holder,
	public std::runtime_error {
public:
	/** Construct standard C library error.
	 * The error message is derived automatically from the value of errno.
	 */
	libc_error();

	/** Construct standard C library error for a specific error number.
	 * @param errno_arg the required error number
	 */
	explicit libc_error(int errno_arg);
};

} /* namespace horace */

#endif
