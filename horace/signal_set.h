// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SIGNAL_SET
#define LIBHOLMES_HORACE_SIGNAL_SET

#include <memory>
#include <csignal>

namespace horace {

/** A class to represent a set of signals. */
class signal_set {
private:
	/** The signal set, in native format. */
	std::unique_ptr<sigset_t> _sigset;
public:
	/** Construct empty signal set. */
	signal_set();

	/** Access signal set in native format.
	 * @return the signal set
	 */
	operator const sigset_t*() const {
		return _sigset.get();
	}

	/** Add signal to signal set (lvalue).
	 * @param signum the signal number
	 */
	signal_set& add(int signum) & {
		sigaddset(_sigset.get(), signum);
		return *this;
	}

	/** Add signal to signal set (rvalue).
	 * @param signum the signal number
	 */
	signal_set add(int signum) && {
		sigaddset(_sigset.get(), signum);
		return std::move(*this);
	}

	/** Mask this signal set. */
	void mask() const;

	/** Wait for one of the signals in this set to be raised.
	 * @return the signal raised
	 */
	int wait() const;
};

/** The set of signals which should terminate the program.
 * These are SIGINT, SIGQUIT and SIGTERM.
 */
extern signal_set terminating_signals;

/** The set of signals which should normally be masked.
 * These are SIGALRM and the members of terminating_signals.
 */
extern signal_set masked_signals;

} /* namespace horace */

#endif
