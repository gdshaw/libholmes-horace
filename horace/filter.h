// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILTER
#define LIBHOLMES_HORACE_FILTER

#include <linux/filter.h>

namespace horace {

/** An abstract base class to represent a packet filter. */
class filter {
public:
	/** Test whether this filter is empty.
	 * @return true if this filter has no effect, otherwise false
	 */
	virtual bool empty() const = 0;

	/** Compile this filter.
	 * The compiled filter remains valid unless and until this function
	 * is called again.
	 * @return the compiled filter.
	 */
	virtual const struct sock_fprog* compile() const = 0;
};

} /* namespace horace */

#endif
