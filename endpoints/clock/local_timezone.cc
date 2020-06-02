// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <errno.h>
#include <time.h>

#include "horace/libc_error.h"

#include "local_timezone.h"

namespace horace {

local_timezone::local_timezone(time_t t) {
	// Get broken-down local time.
	struct tm ltm = {0};
	if (localtime_r(&t, &ltm) == 0) {
		throw libc_error();
	}

	// Convert struct tm back to epoch time,
	// using formula specified by POSIX standard.
	time_t lt = ltm.tm_sec;
	lt += ltm.tm_min * 60;
	lt += ltm.tm_hour * 3600;
	lt += ltm.tm_yday * 86400;
	lt += (ltm.tm_year - 70) * 31536000;
	lt += ((ltm.tm_year - 69) / 4) * 86400;
	lt -= ((ltm.tm_year - 1) / 100) * 86400;
	lt += ((ltm.tm_year + 299) / 400) * 86400;

	// Offset is local time minus universal time,
	// without assuming that time_t is signed, or
	// that the epoch time can be accommodated by
	// a long integer.
	// (It is, however, reasonable to assume that
	// the resulting timezone offset will fit in a
	// long integer.)
	if (lt >= t) {
		_tzoffset = lt - t;
	} else {
		long diff = t - lt;
		_tzoffset = -diff;
	}

	// Get the timezone name/abbreviation.
	// Typically this will be 3 characters long,
	// but technically there is no upper bound
	// so allow a very generous buffer size.
	//
	// A result of 0 might mean buffer overflow,
	// or it might mean that no timezone name
	// is available, but either way the desired
	// behaviour is the same.
	char buffer[64];
	size_t count = strftime(buffer, sizeof(buffer), "%Z", &ltm);
	if (count != 0) {
		_tzname = std::string(buffer, count);
	}
}

} /* namespace horace */
