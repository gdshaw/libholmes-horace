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

	/** Read decimal integer.
	 * @param min_width the minimum width, in characters
	 * @param max_width the maximum width, in characters
	 * @param allow_lsp true if leading spaces allowed, otherwise false
	 * @param allow_lz true if leading zeros allowed, otherwise false
	 */
	unsigned int read_decimal(size_t min_width, size_t max_width,
		bool allow_lsp, bool allow_lz);

	/** Read the priority value.
	 * This must be enclosed within angled brackets, must
	 * be a decimal number between 0 and 255, and must have
	 * no unnecessary leading zeros.
	 * @return the priority value
	 */
	unsigned int read_priority();

	/** Read the version number.
	 * This must be a decimal number between 1 and 999, must
	 * have no leading zeros, and must be followed by a space.
	 * @return the version number
	 */
	unsigned int read_version();

	/** Read a space character. */
	void read_space();

	/** Read a token.
	 * This must be a non-empty string of printable US ASCII
	 * characters, not including any space characters.
	 * @return the token
	 */
	std::string read_token();

	/** Read an RFC 3164 timestamp.
	 * This is required to be of the form "Mmm dd hh:mm:ss".
	 * @return the timestamp
	 */
	std::string read_rfc3164_timestamp();

	/** Read an RFC 5424 STRUCTURED-DATA field.
	 * @return the structured data field content
	 */
	std::string read_rfc5424_structured_data();

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
