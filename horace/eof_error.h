// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EOF_ERROR
#define LIBHOLMES_HORACE_EOF_ERROR

#include <stdexcept>

namespace horace {

/** A class to represent an unexpected end of file. */
class eof_error:
	public std::runtime_error {
public:
	eof_error();
};

} /* namespace horace */

#endif
