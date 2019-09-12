// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_COUNTER
#define LIBHOLMES_HORACE_COUNTER

namespace horace {

/** A class to represent a monotonic-incrementing counter. */
template<typename value_type>
class counter {
private:
	/** The value of the counter. */
	value_type _value;
public:
	/** Construct counter
	 * @param value the required initial value
	 */
	explicit counter(value_type value):
		_value(value) {}

	/** Get value then increment.
	 * @return the value obtained
	 */
	value_type operator()() {
		return _value++;
	}
};

} /* namespace horace */

#endif
