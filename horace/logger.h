// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LOGGER
#define LIBHOLMES_HORACE_LOGGER

#include <memory>
#include <string>
#include <mutex>

namespace horace {

/** An abstract base class for handling log messages.
 * The external interface provided by this class is thread-safe, and
 * can therefore safely handle multiple threads attempting to write
 * to the same log concurrently.
 */
class logger {
public:
	static const int log_emerg = 0;
	static const int log_alert = 1;
	static const int log_crit = 2;
	static const int log_err = 3;
	static const int log_warning = 4;
	static const int log_notice = 5;
	static const int log_info = 6;
	static const int log_debug = 7;
private:
	/** The severity threshold.
	 * Messages with a severity that is numerically less than or
	 * equal to this threshold are enabled.
	 */
	int _severity;

	/** A mutex for controlling concurrent access to the log. */
	std::mutex _log_mutex;
protected:
	/** Write a message to the log.
	 * Implementations of this function are not required to be
	 * thread-safe, therefore the mutex must be locked before
	 * this function is called.
	 * @param msg the message to be written
	 */
	virtual void write(const std::string& msg) = 0;
public:
	/** Construct logger. */
	logger();

	/** Get the severity threshold.
	 * @return the severity
	 */
	int severity() const {
		return _severity;
	}

	/** Set the lowest enabled message severity.
	 * Messages with a severity that is numerically less than or
	 * equal to this threshold are enabled.
	 * @param severity the severity threshold
	 */
	void severity(int severity) {
		_severity = severity;
	}

	/** Determine whether messages with a given severity are enabled.
	 * @param severity the severity level
	 * @return true if enabled, otherwise false
	 */
	bool enabled(int severity) {
		return (severity <= _severity);
	}

	/** Write a message to the log.
	 * This operator is thread-safe.
	 * @param msg the message to be written
	 */
	logger& operator<<(const std::string& msg);
};

/** The log to which messages should be written. */
extern std::unique_ptr<logger> log;

} /* namespace horace */

#endif
