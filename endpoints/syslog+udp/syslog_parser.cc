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
		while ((_index != _length) && isdigit(_content[_index])) {
			unsigned int digit = _content[_index++] - '0';
			if ((count == 0) && (digit == 0)) {
				throw syslog_error("leading zero not permitted");
			}
			priority *= 10;
			priority += digit;
			count += 1;
		}
		if (count < 1) {
			throw syslog_error("digit expected");
		}
		if (count > 3) {
			throw syslog_error("< expected");
		}
		if (priority > 191) {
			throw syslog_error("invalid priority value");
		}
		if ((_index == _length) || (_content[_index++] != '>')) {
			throw syslog_error("'>' expected");
		}
		return priority;
	} catch (std::exception&) {
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
	} catch (syslog_error&) {
		// No action
	}
	builder.add_message(read_remaining());
}

} /* namespace horace */
