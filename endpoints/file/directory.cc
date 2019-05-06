// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/libc_error.h"

#include "directory.h"

namespace horace {

directory::directory(const std::string& pathname):
	_dp(opendir(pathname.c_str())),
	_ep(0) {

	if (!_dp) {
		throw libc_error();
	}
}

directory::~directory() {
	closedir(_dp);
}

directory::operator bool() {
	if (!_ep) {
		_ep = readdir(_dp);
	}
	return _ep;
}

std::string directory::read() {
	if (!_ep) {
		_ep = readdir(_dp);
	}
	if (!_ep) {
		throw std::runtime_error("end of directory");
	}
	const char* filename = _ep->d_name;
	_ep = 0;
	return std::string(filename);
}

} /* namespace horace */
