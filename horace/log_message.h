// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LOG_MESSAGE
#define LIBHOLMES_HORACE_LOG_MESSAGE

#include <sstream>

namespace horace {

class logger;

/** An output stream class to represent a log message. */
class log_message:
	public std::ostringstream {
private:
	/** The logger to which the message should be written. */
	logger* _log;

	/** The message severity. */
	int _severity;
public:
	/** Construct log message.
	 * @param log the logger to which the message should be written
	 * @param severity the message severity
	 */
	log_message(logger& log, int severity);

	/** Write message to log. */
	~log_message();

	/** Get the message severity.
	 * @return the severity
	 */
	int severity() const {
		return _severity;
	}
};

} /* namespace horace */

#endif
