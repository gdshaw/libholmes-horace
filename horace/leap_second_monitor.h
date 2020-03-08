// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LEAP_SECOND_MONITOR
#define LIBHOLMES_HORACE_LEAP_SECOND_MONITOR

#include <memory>
#include <set>
#include <thread>

namespace horace {

/** A class for monitoring packet socket queues during a leap second. */
class leap_second_monitor {
public:
	/** A mixin class for objects requiring leap second monitoring. */
	class measurable {
	public:
		/** Measure state of timestamp queue. */
		virtual void measure() = 0;
	};
private:
	/** The monitoring thread. */
	std::thread _thread;

	/** The objects that require monitoring. */
	std::set<measurable*> _measurables;

	/** Perform any required measurements. */
	void _measure();

	/** The function to be executed by the signing thread. */
	void _run();

	/** Static wrapper function for _run.
	 * @signer the event_signer object
	 */
	static void _do_run(leap_second_monitor& lsm);
public:
	/** Construct leap second monitor. */
	leap_second_monitor();

	/** Stop monitoring. */
	void stop();

	/** Attach object requiring leap second monitoring.
	 * @param obj the object to be attached
	 */
	void attach(measurable& obj);

	/** Detach object requiring leap second monitoring.
	 * @param obj the object to be detached
	 */
	void detach(measurable& obj);
};

/** The active leap second monitor, or 0 if none. */
extern std::unique_ptr<leap_second_monitor> lsmonitor;

} /* namespace horace */

#endif
