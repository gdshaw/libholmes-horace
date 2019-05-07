// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EMPTY_SIGNAL_HANDLER
#define LIBHOLMES_HORACE_EMPTY_SIGNAL_HANDLER

#include <csignal>

namespace horace {

/** A class for installing an empty handler for a given signal number.
 * The purpose of this class is to suppress the default behaviour for a given
 * signal number (which is typically to terminate the process) without
 * fully ignoring the signal using SIG_IGN (which would prevent the process
 * from being interrupted).
 *
 * When an instance of this class is destroyed, the previously applicable
 * signal action is restored.
 */
class empty_signal_handler {
private:
	/** Handle a given signal number, doing nothing.
	 * @param signum the signal number to be handled
	 */
	static void _handle_signal(int signum);

	/** The signal number handled by this instance. */
	int _signum;

	/** The previously-installed handler for _signum. */
	struct sigaction _old_action;
public:
	/** Install empty signal handler.
	 * @param the required signal number
	 */
	empty_signal_handler(int signum);

	/** Restore signal handler. */
	~empty_signal_handler();
};

} /* namespace horace */

#endif
