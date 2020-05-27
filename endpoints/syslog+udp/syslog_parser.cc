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
	_index(0) {}

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
		if ((_index == _length) || (_content[_index++] != ' ')) {
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
		if ((_index == _length) || (_content[_index++] != ' ')) {
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
		if ((_index == _length) || (_content[_index++] != ' ')) {
			throw syslog_error("' ' expected");
		}
		return std::string(_content + restore, _index - restore - 1);
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
	try {
		unsigned int priority = read_priority();
		builder.add_severity(priority & 7);
		builder.add_facility(priority >> 3);

		unsigned int version = 0;
		size_t restore = _index;
		try {
			version = read_version();
		} catch (syslog_error&) {
			_index = restore;
		}

		if (version == 0) {
			try {
				restore = _index;
				std::string timestamp = read_rfc3164_timestamp();
				builder.add_timestamp(timestamp);
				restore = _index;
				std::string hostname = read_token();
				builder.add_hostname(hostname);
			} catch (syslog_error&) {
				_index = restore;
			}
		} else if (version == 1) {
			try {
				restore = _index;
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
			} catch (syslog_error&) {
				_index = restore;
			}
		}
	} catch (syslog_error&) {
		// No action
	}
	builder.add_message(read_remaining());
}

} /* namespace horace */
