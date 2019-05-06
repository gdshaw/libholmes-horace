// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_HOSTNAME
#define LIBHOLMES_HORACE_HOSTNAME

#include <string>

namespace horace {

/** A class to represent the fully-qualified hostname of the local machine. */
class hostname {
private:
	/** The fully qualified hostname. */
	std::string _hostname;
public:
	/** Construct hostname. */
	hostname();

	/** Get hostname. */
	operator const std::string&() const {
		return _hostname;
	}
};

} /* namespace horace */

#endif
