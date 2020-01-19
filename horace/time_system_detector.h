// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TIME_SYSTEM_DETECTOR
#define LIBHOLMES_HORACE_TIME_SYSTEM_DETECTOR

#include <string>

namespace horace {

/** A class for detecting the time system used by the system clock.
 * The current implementation is able to recognise the POSIX and TAI-10
 * time systems, provided that the system has been properly configured to
 * interpret time_t values in that manner.
 *
 * Note that identification of the time system is separate from the
 * question of how accurately the clock has been set (which can only be
 * determined by reference to an external time source).
 */
class time_system_detector {
private:
	/** The detected time system. */
	std::string _time_system;
public:
	/** Detect time system. */
	time_system_detector();

	/** Get the name of the detected time system.
	 * @return the time system name
	 */
	const std::string& time_system() {
		return _time_system;
	}

	/** Validate that a user-requested timezone is consistent with
	 * the system configuration.
	 * @param time_system the user-requested time system
	 * @param leap if non-null, set on exit to true if leap second
	 *  detection required, otherwise set to false
	 */
	void validate(const std::string& time_system, bool* leap);
};

} /* namespace horace */

#endif
