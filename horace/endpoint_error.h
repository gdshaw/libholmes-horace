// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ENDPOINT_ERROR
#define LIBHOLMES_HORACE_ENDPOINT_ERROR

#include <stdexcept>

namespace horace {

/** A class to represent an error originating from an endpoint. */
class endpoint_error:
	public std::runtime_error {
public:
	/** Construct endpoint error.
	 * @param message a message describing the error
	 */
	explicit endpoint_error(const std::string& message):
		std::runtime_error(message) {}
};

} /* namespace horace */

#endif
