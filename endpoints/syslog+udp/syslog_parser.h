// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SYSLOG_PARSER
#define LIBHOLMES_HORACE_SYSLOG_PARSER

namespace horace {

class log_record_builder;

/** A class for parsing syslog messages.
 * In all cases, if a requested component cannot be parsed then a
 * syslog_error is thrown and the state of the parser left unchanged.
 */
class syslog_parser {
private:
	/** The unparsed message content. */
	const char* _content;

	/** The length of the unparsed message content. */
	size_t _length;

	/** An index into the unparsed message content. */
	size_t _index;
public:
	/** Construct Syslog message parser from string.
	 * @param content the unparsed message content
	 * @param length the length of the unparsed message content
	 */
	syslog_parser(const char* content, size_t length);

	/** Read the priority value.
	 * This must be enclosed within angled brackets, must
	 * be a decimal number between 0 and 255, and must have
	 * no leading zeros.
	 * @return the priority value
	 */
	unsigned int read_priority();

	/** Read the remainder of the message.
	 * @return the remainder of the message
	 */
	std::string read_remaining();

	/** Read full message, pass to log record builder.
	 * @param builder the log record builder
	 */
	void read_message(log_record_builder& builder);
};

} /* namespace horace */

#endif
