// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RETRY_EXCEPTION
#define LIBHOLMES_HORACE_RETRY_EXCEPTION

#include <stdexcept>

namespace horace {

/** An exception class for retrying a failed data transfer. */
class retry_exception:
	public std::exception {
public:
	/** Construct retry exception. */
	retry_exception() {}

	/** Construct chained retry exception. */
	retry_exception(const std::exception& that):
		std::exception(that) {}

	virtual const char* what() const noexcept;
};

} /* namespace horace */

#endif
