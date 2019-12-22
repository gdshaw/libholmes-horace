// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>
#include <climits>
#include <sstream>
#include <iomanip>

#include "spoolfile.h"

namespace horace {

spoolfile::spoolfile(const std::string& filename):
	_filename(filename),
	_filenum(0) {

	if (_filename.empty()) {
		throw std::invalid_argument("invalid spoolfile name");
	}
	for (auto it = _filename.begin(); it != _filename.end(); ++it) {
		char c = *it;
		if ((c < '0') || (c > '9')) {
			throw std::invalid_argument("invalid spoolfile name");
		}
		unsigned long digit = c - '0';
		if (_filenum > ULONG_MAX / 10) {
			throw std::out_of_range("filenum overflow");
		}
		_filenum *= 10;
		if (_filenum > ULONG_MAX - digit) {
			throw std::out_of_range("filenum overflow");
		}
		_filenum += digit;
	}
}

spoolfile::spoolfile(unsigned long filenum, unsigned int minwidth):
	_filenum(filenum) {

	std::ostringstream builder;
	builder << std::setfill('0') << std::setw(minwidth) << _filenum;
	_filename = builder.str();
}

} /* namespace */
