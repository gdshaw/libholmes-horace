// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sys/timex.h>

#include "horace/terminate_flag.h"
#include "horace/log_message.h"
#include "horace/logger.h"
#include "horace/leap_second_monitor.h"

namespace horace {

static const long seconds_per_day = 86400;
static const long first_second_of_day = 0;
static const long last_second_of_day = seconds_per_day - 1;

void leap_second_monitor::_measure() {
	for (measurable* obj : _measurables) {
		obj->measure();
	}
}

void leap_second_monitor::_run() {
	while (true) {
		// Calculate the number of milliseconds since the start of the
		// current POSIX day (rounding down).
		struct timex tmx;
		tmx.modes = 0;
		int clock_state = ntp_adjtime(&tmx);
		if (tmx.status & STA_NANO) {
			tmx.time.tv_usec /= 1000;
		}
		long tod_sec = tmx.time.tv_sec % seconds_per_day;
		long tod_msec = tod_sec * 1000 + tmx.time.tv_usec / 1000;

		// Aim to take three measurements at 23:59:59.250 UTC,
		// 23:59:59.750 UTC and 23:59:60.250 UTC (or in the case
		// this is not a leap second, 00:00:00.250 UTC). This is one
		// more than is needed, but widens the safety margin.
		//
		// If the correct time to start this sequence has recently
		// been missed then start immediately with the same spacing,
		// otherwise wait for the appropriate number of milliseconds.
		//
		// There is potential for leap seconds to be missed if there
		// is a step change in the clock during the day before, however
		// if this were to happen during a session then it would cause
		// much larger problems with timestamp accuracy.
		long rem_msec = seconds_per_day * 1000 - tod_msec;
		if ((tod_msec >= 1000) && (rem_msec > 750) &&
			(clock_state != TIME_OOP)) {
			terminating.millisleep(rem_msec - 750);
		}

		// Perform the first measurement, then wait 0.5 seconds
		_measure();
		terminating.millisleep(500);

		// Perform the second measurement, then wait 0.5 seconds
		_measure();
		terminating.millisleep(500);

		// Perform the third measurement, then wait 2 seconds.
		_measure();
		terminating.millisleep(2000);
	}
}

void leap_second_monitor::_do_run(leap_second_monitor& lsm) {
	try {
		lsm._run();
	} catch (terminate_exception& ex) {
		// no action
	} catch (std::exception& ex) {
		if (log->enabled(logger::log_err)) {
			log_message msg(*log, logger::log_err);
			msg << ex.what();
		}
	}
}

leap_second_monitor::leap_second_monitor() {
	_thread = std::thread(_do_run, std::ref(*this));
}

void leap_second_monitor::stop() {
	_thread.join();
}

void leap_second_monitor::attach(measurable& obj) {
	_measurables.insert(&obj);
}

void leap_second_monitor::detach(measurable& obj) {
	_measurables.erase(&obj);
}

std::unique_ptr<leap_second_monitor> lsmonitor;

} /* namespace horace */
