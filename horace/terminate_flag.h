// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TERMINATE_FLAG
#define LIBHOLMES_HORACE_TERMINATE_FLAG

#include <atomic>

#include "horace/terminate_exception.h"

namespace horace {

/** A class for indicating that the process is terminating. */
class terminate_flag {
private:
	/** True if process terminating, otherwise false. */
	std::atomic_bool _terminating;
public:
	/** Construct termination flag. */
	terminate_flag():
		_terminating(false) {}

	/** Set termination flag. */
	void set() {
		_terminating = true;
	}

	/** Poll termination flag.
	 * Throw a terminate_exception if the flag is set.
	 */
	void poll() {
		if (_terminating) {
			throw terminate_exception();
		}
	}
};

/** A flag to indicate that the process is terminating. */
extern terminate_flag terminating;

} /* namespace horace */

#endif
