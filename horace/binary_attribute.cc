// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/octet_reader.h"
#include "horace/binary_attribute.h"

namespace horace {

binary_attribute::binary_attribute(int attrid, size_t length, const void* content):
	binary_ref_attribute(attrid, length, new char[length]) {

	char* _content = const_cast<char*>(
		static_cast<const char*>(this->content()));
	memcpy(_content, content, length);
}

binary_attribute::binary_attribute(int attrid, size_t length, octet_reader& in):
	binary_ref_attribute(attrid, length, new char[length]) {

	char* _content = const_cast<char*>(
		static_cast<const char*>(content()));
	try {
		in.read(_content, length);
	} catch (...) {
		delete[] _content;
		throw;
	}
}

std::unique_ptr<attribute> binary_attribute::clone() const {
	return std::make_unique<binary_attribute>(
		attrid(), length(), content());
}

binary_attribute::~binary_attribute() {
	// Note that the underlying binary_ref_attribute still has a
	// pointer to the content as this point, however it does not
	// (and has no reason to) dereference it during destruction.
	char* _content = const_cast<char*>(
		static_cast<const char*>(content()));
	delete[] _content;
}

} /* namespace horace */
