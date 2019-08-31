// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/binary_attribute.h"

namespace horace {

binary_attribute::binary_attribute(int type, size_t length, const void* content):
	binary_ref_attribute(type, length, new char[length]) {

	char* _content = const_cast<char*>(
		static_cast<const char*>(this->content()));
	memcpy(_content, content, length);
}

binary_attribute::binary_attribute(int type, size_t length, octet_reader& in):
	binary_ref_attribute(type, length, new char[length]) {

	char* _content = const_cast<char*>(
		static_cast<const char*>(content()));
	try {
		in.read(_content, length);
	} catch (...) {
		delete[] _content;
		throw;
	}
}

binary_attribute::~binary_attribute() {
	// Note that the underlying binary_ref_attribute still has a
	// pointer to the content as this point, however it does not
	// and has no reason to dereference it during destruction.
	char* _content = const_cast<char*>(
		static_cast<const char*>(content()));
	delete[] _content;
}

} /* namespace horace */
