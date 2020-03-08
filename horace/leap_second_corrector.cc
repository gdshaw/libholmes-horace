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

leap_second_corrector::leap_second_corrector():
	_index(0) {}

void leap_second_corrector::correct(struct timespec& ts) {
	// Increment the timestamp index now (in case this function
	// returns early).
	_index += 1;

	// Discard any expired queue state measurements.
	// This is done prior to any point where this function might
	// return early, to keep the measurement queue short.
	//
	// If there are any measurements remaining in the queue afterwards
	// then the first of these must necessarily apply to the timestamp
	// that is currently being processed, and must be the tighest bound
	// available.
	while (!_clock_states.empty() && (_clock_states.front().index < _index)) {
		_clock_states.pop();
	}

	// Leap seconds need only be considered if the timestamp is within
	// the last second of a day, or perhaps for a very brief period
	// within the first second of a day.
	int second_of_day = ts.tv_sec % seconds_per_day;
	if ((second_of_day != last_second_of_day) &&
		(second_of_day != first_second_of_day)) {

		return;
	}

	// Get the clock state for a reference time known not to pre-date
	// the timestamp to be corrected.
	struct timeval reftime;
	int refstate;
	if (!_clock_states.empty()) {
		// By preference use a clock state previously recorded
		// and queued, because this will provide a tighter bound.
		refstate = _clock_states.front().state;
		reftime = _clock_states.front().time;
	} else {
		// Otherwise, record the clock state now.
		struct timex tmx;
		tmx.modes = 0;
		refstate = ntp_adjtime(&tmx);
		if (tmx.status & STA_NANO) {
			tmx.time.tv_usec /= 1000;
		}
		reftime = tmx.time;
	}

	// The following tests work on the assumption that if timestamp to
	// to corrected could have been recorded either t seconds or t+1
	// seconds prior to the reference timestamp, then the more recent
	// option is more likely.
	//
	// It follows that if the reference timestamp follows the timestamp
	// to be corrected by less than one second, the correct outcome is
	// assured.
	if (refstate == TIME_WAIT) {
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
	} else if (refstate == TIME_OOP) {
		// A leap second is in progress (but might not have been
		// in progress when the packet was timestamped).

		// Clock precision is in microseconds, therefore will want
		// to perform timestamp comparison in microseconds.
		// (It is better that similar times be declared equal than
		// risk reversing their comparison order.)
		struct timeval uts = {
			.tv_sec = ts.tv_sec,
			.tv_usec = ts.tv_nsec / 1000 };

		if ((uts.tv_sec > reftime.tv_sec) ||
			((uts.tv_sec == reftime.tv_sec) &&
			(uts.tv_usec > reftime.tv_usec))) {

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
	} else if (refstate == TIME_INS) {
		if (second_of_day == first_second_of_day) {
			// The packet was timestamped during the
			// leap second, but before the leap second
			// was applied to the clock.
			ts.tv_sec -= 1;
			ts.tv_nsec += leap_second_indicator;
		}
	}
}

void leap_second_corrector::record_clock_state(size_t index) {
	// Get the current clock state.
	struct timex tmx;
	tmx.modes = 0;
	int state = ntp_adjtime(&tmx);
	if (tmx.status & STA_NANO) {
		tmx.time.tv_usec /= 1000;
	}

	// Record the queue state measurement.
	clock_state cs = {
		.index = index,
		.time = tmx.time,
		.state = state };
	_clock_states.push(cs);
}

bool detect_leap_seconds = false;

} /* namespace horace */
