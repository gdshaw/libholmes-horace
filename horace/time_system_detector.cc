// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/time_system_detector.h"

namespace horace {

// This class operates by attempting to convert a time_t value of 946684800
// to UTC using the gmtime function. A time in the past is used so that the
// leap-second offset between UTC and TIA is fixed and known (32 seconds).
//
// A result of 2000-01-01 00:00:00 indicates that the time_t is being
// interpreted as UTC. Since leap second detection is not yet implemented,
// the time system is reported to be "POSIX".
//
// A result of 1999-12-31 23:59:38 indicates that it is being interpreted
// as TAI minus 10 seconds (which is equivalent to using the POSIX epoch of
// 1970-01-01 00:00:00 UTC, but including leap seconds in the count). This
// outcome is not POSIX-compliant, however it is how the gmtime function
// behaves when using the "right" configuration of the IANA Time Zone
// Database. It is reported as "TAI-10".
//
// Any other value would be unusual and is not recognised.
//
// Current behaviour takes account of the TZ environment variable, however
// it is arguable that this should be explicitly unset in order to query
// the default timezone.

time_system_detector::time_system_detector() {
	time_t t = (30 * 365 + 7) * 86400;
	struct tm tms = {0};
	gmtime_r(&t, &tms);

	if ((tms.tm_sec == 0) && (tms.tm_min == 0) &&
		(tms.tm_hour == 0) && (tms.tm_mday == 1) &&
		(tms.tm_mon == 0) && (tms.tm_year == 100) &&
		(tms.tm_isdst == 0)) {

		_time_system = "POSIX";
	} else if ((tms.tm_sec == 38) && (tms.tm_min == 59) &&
		(tms.tm_hour == 23) && (tms.tm_mday == 31) &&
		(tms.tm_mon == 11) && (tms.tm_year == 99) &&
		(tms.tm_isdst == 0)) {

		_time_system = "TAI-10";
	} else {
		throw horace_error("time system not recognised");
	}
}

void time_system_detector::validate(const std::string& time_system, bool* leap) {
	if (time_system == "POSIX") {
		if (_time_system != time_system) {
			throw horace_error("system configured to use " + _time_system + " time");
		}
		if (leap) {
			*leap = false;
		}
	} else if (time_system == "UTC") {
		if (_time_system != "POSIX") {
			throw horace_error("system configured to use " + _time_system + " time");
		}
		if (leap) {
			*leap = true;
		}
	} else if (time_system == "TAI-10") {
		if (_time_system != time_system) {
			throw horace_error("system configured to use " + _time_system + " time");
		}
		if (leap) {
			*leap = false;
		}
	} else {
		throw horace_error("time system not recognised");
	}
}

} /* namespace horace */
