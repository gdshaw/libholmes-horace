// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LEAP_SECOND_CORRECTOR
#define LIBHOLMES_HORACE_LEAP_SECOND_CORRECTOR

#include <cstdint>
#include <queue>

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
private:
	/** The current timestamp index.
	 * This is equal to the number of timestamps which have been
	 * presented to this leap second corrector for correction
	 * since construction. It is required and presumed to increase
	 * monotonically.
	 */
	uint64_t _index;

	/** A struct for recording information about the state of the clock
	 * corresponding to a given timestamp index.
	 */
	struct clock_state {
		/** The lowest unused timestamp index.
		 * Unused means the corresponding timestamp has not already
		 * been processed, and is not known to have been queued.
		 */
		uint64_t index;

		/** The time indicated by the clock. */
		struct timeval time;

		/** The clock state, in the form returned by ntp_gettime. */
		int state;
	};

	/** A queue of clock state measurements relevant to unprocessed
	 * timestamps. */
	std::queue<clock_state> _clock_states;
public:
	/** Construct leap second corrector. */
	leap_second_corrector();

	/** Correct timestamp.
	 * @param ts the timestamp to be corrected
	 */
	void correct(struct timespec& ts);

	/** Record clock state.
	 * This function provides the leap second corrector with the
	 * additional information that any timestamps indexed prior to
	 * the point specified have already been queued by the kernel.
	 *
	 * It follows that the true values of those timestamps cannot have
	 * been later than the moment in time at which this function was
	 * called.
	 *
	 * This function may be called at any time, however there is no
	 * benefit in doing so unless there is a leap second approaching.
	 * Two measurements are sufficient per leap second if they are
	 * appropriately timed:
	 * - One before the start of the leap second.
	 * - One less than a second later, but during the leap second.
	 *
	 * It is necessary to allow for the fact that a finite amount of
	 * time to measure the queue length and inspect the system clock,
	 * and that it will not be possible to precisely schedule when
	 * they will occur. However, the following process would be
	 * sufficient to allow for any reasonable delays:
	 * - Start the first measurement as soon as possible after the
	 *   true UTC value of tm_sec has passed 59.5.
	 * - Start the second measurement as soon as possible after it
	 *   has passed 60.0.
	 *
	 * @param index the lowest unused timestamp index
	 */
	void record_clock_state(uint64_t index);
};

extern bool detect_leap_seconds;

} /* namespace horace */

#endif
