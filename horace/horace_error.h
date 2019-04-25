// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_HORACE_ERROR
#define LIBHOLMES_HORACE_HORACE_ERROR

#include <stdexcept>

namespace horace {

/** A class to represent an error relating to the HORACE protocol. */
class horace_error:
	public std::runtime_error {
public:
	/** Construct HORACE error. */
	horace_error(const std::string& message):
		std::runtime_error(message) {}
};

} /* namespace horace */

#endif
