// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <errno.h>
#include <limits.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "horace/libc_error.h"

#include "local_timezone.h"

namespace horace {

local_timezone::local_timezone(time_t t):
	_t(t) {

	if (localtime_r(&_t, &_ltm) == 0) {
		throw libc_error();
	}

	_init_tzoffset();
	_init_tzabbrev();
	_init_tzname();
}

void local_timezone::_init_tzoffset() {
	// Convert struct tm back to epoch time,
	// using formula specified by POSIX standard.
	time_t lt = _ltm.tm_sec;
	lt += _ltm.tm_min * 60;
	lt += _ltm.tm_hour * 3600;
	lt += _ltm.tm_yday * 86400;
	lt += (_ltm.tm_year - 70) * 31536000;
	lt += ((_ltm.tm_year - 69) / 4) * 86400;
	lt -= ((_ltm.tm_year - 1) / 100) * 86400;
	lt += ((_ltm.tm_year + 299) / 400) * 86400;

	// Offset is local time minus universal time,
	// without assuming that time_t is signed, or
	// that the epoch time can be accommodated by
	// a long integer.
	// (It is, however, reasonable to assume that
	// the resulting timezone offset will fit in a
	// long integer.)
	if (lt >= _t) {
		_tzoffset = lt - _t;
	} else {
		long diff = _t - lt;
		_tzoffset = -diff;
	}
}

void local_timezone::_init_tzabbrev() {
	// Typically the abbreviation will be 3 characters
	// long, but technically there is no upper bound
	// so allow a very generous buffer size.
	//
	// A result of 0 might mean buffer overflow, or it
	// might mean that no timezone name is available,
	// but either way the desired behaviour is the same.
	char buffer[64];
	size_t count = strftime(buffer, sizeof(buffer), "%Z", &_ltm);
	if (count != 0) {
		_tzabbrev = std::string(buffer, count);
	}
}

void local_timezone::_init_tzname() {
	// This is not a fully portable method, but works
	// well enough to be useful and is unlikely to deliver
	// false positives.
	char buffer[PATH_MAX];
	ssize_t count = readlink("/etc/localtime", buffer, sizeof(buffer));
	if ((count > 0) && (count < sizeof(buffer))) {
		buffer[count] = 0;
		const char* zoneinfo = "/zoneinfo/";
		const char* f = strstr(buffer, zoneinfo);
		if (f) {
			_tzname = std::string(f + strlen(zoneinfo));
		}
	}
}

} /* namespace horace */
