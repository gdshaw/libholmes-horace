// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cctype>
#include <string>

#include "horace/log_record_builder.h"

#include "syslog_error.h"
#include "syslog_parser.h"

namespace horace {

syslog_parser::syslog_parser(const char* content, size_t length):
	_content(content),
	_length(length),
	_index(0) {}

unsigned int syslog_parser::read_priority() {
	size_t restore = _index;
	try {
		if ((_index == _length) || (_content[_index++] != '<')) {
			throw syslog_error("'<' expected");
		}

		unsigned int priority = 0;
		size_t count = 0;
		size_t leading = 0;
		while ((_index != _length) && isdigit(_content[_index])) {
			unsigned int digit = _content[_index++] - '0';
			priority *= 10;
			priority += digit;
			if ((count == leading) && (digit == 0)) {
				leading += 1;
			}
			count += 1;
		}
		if (count < 1) {
			throw syslog_error("digit expected");
		}
		if (count > 3) {
			throw syslog_error("'<' expected");
		}
		if ((leading != 0) && ((priority != 0) || (leading > 1))) {
			throw syslog_error("invalid leading zero");
		}
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
		unsigned int version = 0;
		size_t count = 0;
		size_t leading = 0;
		while ((_index != _length) && isdigit(_content[_index])) {
			unsigned int digit = _content[_index++] - '0';
			version *= 10;
			version += digit;
			if ((count == leading) && (digit == 0)) {
				leading += 1;
			}
			count += 1;
		}
		if (count < 1) {
			throw syslog_error("digit expected");
		}
		if (count > 3) {
			throw syslog_error("' ' expected");
		}
		if (leading != 0) {
			throw syslog_error("invalid leading zero");
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

		size_t restore = _index;
		try {
			unsigned int version = read_version();
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
	} catch (syslog_error&) {
		// No action
	}
	builder.add_message(read_remaining());
}

} /* namespace horace */
