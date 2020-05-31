// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cctype>
#include <cstring>
#include <string>

#include "horace/log_record_builder.h"

#include "syslog_error.h"
#include "syslog_parser.h"

namespace horace {

syslog_parser::syslog_parser(const char* content, size_t length):
	_content(content),
	_length(length),
	_index(0) {

	// Treat any terminating CR, LF or CRLF as framing to be ignored.
	if ((_length != 0) && (_content[_length - 1] == '\n')) {
		_length--;
	}
	if ((_length != 0) && (_content[_length - 1] == '\r')) {
		_length--;
	}
}

unsigned int syslog_parser::read_decimal(size_t min_width, size_t max_width,
	bool allow_lsp, bool allow_lz) {

	size_t restore = _index;
	try {
		if (max_width > _length - _index) {
			max_width = _length - _index;
		}

		unsigned int value = 0;
		size_t count = 0;
		size_t lsp = 0;
		size_t lz = 0;

		if (allow_lsp) {
			while ((count < max_width) && (_content[_index] == ' ')) {
				_index += 1;
				lsp += 1;
				count += 1;
			}
		}

		while ((count < max_width) && isdigit(_content[_index])) {
			char digit = _content[_index++] - '0';
			if (digit == 0) {
				if (count == lsp + lz) {
					lz += 1;
				}
			}
			value *= 10;
			value += digit;
			count += 1;
		}

		if ((count == lsp) || (count < min_width)) {
			throw syslog_error("digit expected");
		}
		if (!allow_lz) {
			if ((lz != 0) && ((value != 0) || (lz > 1))) {
				throw syslog_error("invalid leading zero");
			}
		}
		return value;
	} catch (syslog_error&) {
		_index = restore;
		throw;
	}
}

unsigned int syslog_parser::read_priority() {
	size_t restore = _index;
	try {
		if ((_index == _length) || (_content[_index++] != '<')) {
			throw syslog_error("'<' expected");
		}

		unsigned int priority = read_decimal(1, 3, false, false);
		if (priority > 191) {
			throw syslog_error("invalid priority value");
		}
		if ((_index == _length) || (_content[_index++] != '>')) {
			throw syslog_error("'>' expected");
		}
		return priority;
	} catch (syslog_error&) {
		_index = restore;
		throw;
	}
}

unsigned int syslog_parser::read_version() {
	size_t restore = _index;
	try {
		unsigned int version = read_decimal(1, 3, false, false);
		if (version == 0) {
			throw syslog_error("invalid version number");
		}
		if ((_index != _length) && (_content[_index++] != ' ')) {
			throw syslog_error("' ' expected");
		}
		return version;
	} catch (syslog_error& ex) {
		_index = restore;
		throw;
	}
}

std::string syslog_parser::read_token() {
	size_t restore = _index;
	try {
		size_t base = _index;
		while ((_index != _length) && isgraph(_content[_index])) {
			_index += 1;
		}
		if (_index == base) {
			throw syslog_error("token expected");
		}
		std::string token(_content + base, _index - base);
		if ((_index != _length) && (_content[_index++] != ' ')) {
			throw syslog_error("' ' expected");
		}
		return token;
	} catch (syslog_error&) {
		_index = restore;
		throw;
	}
}

std::string syslog_parser::read_rfc3164_timestamp() {
	size_t restore = _index;
	try {
		if (_index + 15 > _length) {
			throw syslog_error("timestamp expected");
		}
		static const char* month_names = "JanFebMarAprMayJunJulAugSepOctNovDec";
		char month_name[4];
		month_name[0] = _content[_index++];
		month_name[1] = _content[_index++];
		month_name[2] = _content[_index++];
		month_name[3] = 0;
		const char* f = strstr(month_names, month_name);
		if ((!f) || ((f - month_names) % 3)) {
			throw syslog_error("month name expected");
		}
		unsigned int month = (f - month_names) / 3;
		if (_content[_index++] != ' ') {
			throw syslog_error("' ' expected");
		}

		unsigned int day = read_decimal(2, 2, true, false);
		static const unsigned int month_lengths[] = {
			31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
		if ((day < 1) || (day > month_lengths[month])) {
			throw syslog_error("invalid day of month");
		}
		if (_content[_index++] != ' ') {
			throw syslog_error("' ' expected");
		}

		unsigned int hour = read_decimal(2, 2, false, true);
		if (hour > 59) {
			throw syslog_error("invalid hour");
		}
		if (_content[_index++] != ':') {
			throw syslog_error("':' expected");
		}

		unsigned int minute = read_decimal(2, 2, false, true);
		if (minute > 59) {
			throw syslog_error("invalid minute");
		}
		if (_content[_index++] != ':') {
			throw syslog_error("':' expected");
		}

		unsigned int second = read_decimal(2, 2, false, true);
		if (second > 59) {
			throw syslog_error("invalid second");
		}

		std::string timestamp(_content + restore, _index - restore);
		if ((_index != _length) && (_content[_index++] != ' ')) {
			throw syslog_error("' ' expected");
		}
		return timestamp;
	} catch (syslog_error&) {
		_index = restore;
		throw;
	}
}

std::string syslog_parser::read_rfc5424_structured_data() {
	size_t restore = _index;
	try {
		if ((_index != _length) && (_content[_index] == '-')) {
			_index++;
			if ((_index != _length) && (_content[_index++] != ' ')) {
				throw syslog_error("' ' expected");
			}
			return "-";
		}

		if ((_index == _length) || (_content[_index++] != '[')) {
			throw syslog_error("'[' expected");
		}

		bool quote = false;
		while ((_index != _length) && (_content[_index] != ']')) {
			char ch = _content[_index++];
			if (ch == '"') {
				quote = !quote;
			} else if (ch == '\\') {
				if (quote) {
					if (_index == _length) {
						throw syslog_error("']' expected (2)");
					}
					++_index;
				}
			}
		}
		if (quote) {
			throw syslog_error("'\"' expected");
		}
		if ((_index == _length) || (_content[_index++] != ']')) {
			throw syslog_error("']' expected *3(");
		}
		std::string data(_content + restore, _index - restore);
		if ((_index != _length) && (_content[_index++] != ' ')) {
			throw syslog_error("' ' expected");
		}
		return data;
	} catch (syslog_error&) {
		_index = restore;
		throw;
	}
}

std::string syslog_parser::read_remaining() {
	std::string msg(_content + _index, _length - _index);
	_index = _length;
	return msg;
}

void syslog_parser::read_message(log_record_builder& builder) {
	// Initialise the restore point.
	// This is the number of characters from the original message
	// that have been parsed, converted into attributes, and passed
	// to the record builder.
	// This includes the terminating space following the most recent
	// attribute, if one was permitted and observed.
	size_t restore = _index;
	try {
		// Read the priority, split into severity and facility.
		unsigned int priority = read_priority();
		builder.add_priority(priority);
		restore = _index;

		// Read the protocol version number if there is one
		// (but it is not an error if there is not).
		// Since a version number number of 0 is not allowed,
		// use that to represent the absence of a version number.
		unsigned int version = 0;
		try {
			version = read_version();
		} catch (syslog_error&) {
			_index = restore;
		}

		if (version == 0) {
			// No version: assume RFC 3164.
			// Parse the timestamp.
			std::string timestamp = read_rfc3164_timestamp();
			builder.add_timestamp(timestamp);
			restore = _index;

			// Parse the hostname.
			std::string hostname = read_token();
			builder.add_hostname(hostname);
			restore = _index;
		} else if (version == 1) {
			// Version 1: RFC 5424
			// Parse the header (all or nothing).
			std::string timestamp = read_token();
			std::string hostname = read_token();
			std::string appname = read_token();
			std::string procid = read_token();
			std::string msgid = read_token();
			builder.add_version(version);
			builder.add_timestamp(timestamp);
			builder.add_hostname(hostname);
			builder.add_appname(appname);
			builder.add_procid(procid);
			builder.add_msgid(msgid);
			restore = _index;

			// Parse the structured data.
			// (This ought to be present, but is more likely to
			// be malformed.)
			std::string sd = read_rfc5424_structured_data();
			builder.add_structured_data(sd);
			restore = _index;
		}
	} catch (syslog_error&) {
		_index = restore;
	}

	// Encode any remaining characters as a message attribute.
	// So that the original message can be reconstructed:
	// - If the previous field was terminated by a space then
	//   a message attribute is required (even if it is empty),
	//   and the message attribute does not include the space.
	// - Otherwise, a message attribute is required only if it
	//   would be non-empty.
	if ((_index != 0) && (_content[_index - 1] == ' ')) {
		builder.add_message(read_remaining());
	} else if (_index != _length) {
		builder.add_message(read_remaining());
	}
}

} /* namespace horace */
