// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LEAP_SECOND_CORRECTOR
#define LIBHOLMES_HORACE_LEAP_SECOND_CORRECTOR

namespace horace {

/** A class for correcting timestamps for leap seconds.
 * Given a recently-recorded POSIX-compatible timespec, this class will
 * attempt to convert it to UTC with a leap second indicator encoded into
 * the tv_nsec field (by adding 1E9 to the value).
 * The conversion makes three assumptions:
 * - That the timespec was recorded no more than 1 second prior to the
 *   correction.
 * - That leap second announcements are being delivered to the system
 *   clock (as opposed, for example, to smearing the leap second upstream
 *   of the system clock).
 * - That leap seconds are applied by the system clock using the method
 *   described by Mills.
 */
class leap_second_corrector {
public:
	/** Correct timestamp.
	 * @param ts the timestamp to be corrected
	 */
	void correct(struct timespec& ts);
};

extern bool detect_leap_seconds;

} /* namespace horace */

#endif
