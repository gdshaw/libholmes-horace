// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TERMINATE_EXCEPTION
#define LIBHOLMES_HORACE_TERMINATE_EXCEPTION

#include <stdexcept>

namespace horace {

/** An exception class for indicating to a thread that it
 * should terminate. */
class terminate_exception:
	public std::exception {
public:
	/** Construct terminate exception. */
	terminate_exception() {}

	virtual const char* what() const noexcept;
};

} /* namespace horace */

#endif
