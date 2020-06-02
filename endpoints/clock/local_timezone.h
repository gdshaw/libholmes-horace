// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LOCAL_TIMEZONE
#define LIBHOLMES_HORACE_LOCAL_TIMEZONE

#include <string>

namespace horace {

/** A class for querying the local timezone. */
class local_timezone {
private:
	/** The offset from UTC to local time, in seconds. */
	long _tzoffset;

	/** The name of the local timezone. */
	std::string _tzname;
public:
	/** Query local timezone.
	 * @param t the epoch time for which to perform the query
	 */
	local_timezone(time_t t);

	/** Get the offset from UTC to local time.
	 * @return the offset, in seconds
	 */
	long tzoffset() const {
		return _tzoffset;
	}

	/** Get the name of the local timezone.
	 * @return the name
	 */
	const std::string& tzname() const {
		return _tzname;
	}
};

} /* namespace horace */

#endif
