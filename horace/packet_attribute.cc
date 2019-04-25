// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>
#include <cstring>

#include "horace/packet_attribute.h"

namespace horace {

packet_attribute::packet_attribute(octet_reader& in, size_t length):
	packet_ref_attribute(new char[length], length) {

	char* _content = const_cast<char*>(
		reinterpret_cast<const char*>(content()));
	try {
		in.read(_content, length);
	} catch (...) {
		delete[] _content;
		throw;
	}
}

packet_attribute::~packet_attribute() {
	// Note that the underlying packet_ref_attribute still has a
	// pointer to the packet content as this point, however it does
	// not and has no reason to dereference it during destruction.
	char* _content = const_cast<char*>(
		reinterpret_cast<const char*>(content()));
	delete[] _content;
}

} /* namespace horace */
