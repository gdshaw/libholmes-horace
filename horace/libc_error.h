// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LIBC_ERROR
#define LIBHOLMES_HORACE_LIBC_ERROR

#include <stdexcept>

namespace horace {

/** A class to represent an error corresponding to a standard C library
 * error number. */
class libc_error:
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
