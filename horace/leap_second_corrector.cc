// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <time.h>
#include <sys/timex.h>

#include "horace/leap_second_corrector.h"

namespace horace {

static const long seconds_per_day = 86400;
static const long first_second_of_day = 0;
static const long last_second_of_day = seconds_per_day - 1;
static const long leap_second_indicator = 1000000000;

void leap_second_corrector::correct(struct timespec& ts) {
	// The following tests presume that the packet has not been
	// buffered for more than a second (a condition which might be
	// testable, but is not currently tested).
	//
	// Leap seconds need only be considered if the timestamp is within
	// the last second of a day, or perhaps for a very brief period
	// within the first second of a day.
	int second_of_day = ts.tv_sec % seconds_per_day;
	if ((second_of_day != last_second_of_day) &&
		(second_of_day != first_second_of_day)) {

		return;
	}

	// Get the clock state, converting the current time to
	// nanoseconds if necessary.
	struct ntptimeval ntv;
	int state = ntp_gettime(&ntv);

	if (state == TIME_WAIT) {
		// A leap second was recently completed (but might not
		// have been completed when the packet was timestamped).
		if (second_of_day == last_second_of_day) {
			// The packet was timestamped either during the
			// leap second or more than 1 second ago.
			// Assume the former.
			ts.tv_nsec += leap_second_indicator;
		} else {
			// The packet was timestamped after the end of
			// the leap second.
		}
	} else if (state == TIME_OOP) {
		// A leap second is in progress (but might not have been
		// in progress when the packet was timestamped).

		// Clock precision is in microseconds, therefore will want
		// to perform timestamp comparison in microseconds.
		// (It is better that similar times be declared equal than
		// risk reversing their comparison order.)
		struct timeval uts = {
			.tv_sec = ts.tv_sec,
			.tv_usec = ts.tv_nsec / 1000 };

		if ((uts.tv_sec > ntv.time.tv_sec) ||
			((uts.tv_sec == ntv.time.tv_sec) &&
			(uts.tv_usec > ntv.time.tv_usec))) {

			// The timestamp post-dates the current time.
			if (second_of_day == last_second_of_day) {
				// The packet was timestamped before the
				// start of the leap second.
			} else {
				// The packet was timestamped during the
				// leap second, but before the leap second
				// was applied to the clock.
				ts.tv_sec -= 1;
				ts.tv_nsec += leap_second_indicator;
			}
		} else {
			// The timestamp does not post-date the current
			// time.
			if (second_of_day == last_second_of_day) {
				// The packet was timestamped either during
				// the leap second or more than 1 second
				// ago. Assume the former.
				ts.tv_nsec += leap_second_indicator;
			} else {
				// The packet was timestamped either after
				// the end of the leap second or more than
				// 1 second ago. Assume the former.
			}
		}
	} else if (state == TIME_INS) {
		if (second_of_day == first_second_of_day) {
			// The packet was timestamped during the
			// leap second, but before the leap second
			// was applied to the clock.
			ts.tv_sec -= 1;
			ts.tv_nsec += leap_second_indicator;
		}
	}
}

bool detect_leap_seconds = false;

} /* namespace horace */
