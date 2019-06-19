// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iomanip>

#include "eui_attribute.h"

namespace horace {

eui_attribute::eui_attribute(const std::string& eui):
	_eui(eui) {}

eui_attribute::eui_attribute(octet_reader& in, size_t length):
	_eui(in.read_string(length)) {}

int eui_attribute::type() const {
	return ATTR_EUI;
}

size_t eui_attribute::length() const {
	return _eui.length();
}

void eui_attribute::write(std::ostream& out) const {
	out << "eui(" << std::hex << std::uppercase;
	bool first = true;
	for (char ch : _eui) {
		if (first) {
			first = false;
		} else {
			out << "-";
		}
		unsigned int b = static_cast<unsigned int>(ch) & 0xff;
		out << std::setw(2) << b;
	}
	out << std::dec;
}

void eui_attribute::write(octet_writer& out) const {
	out.write_base128(type());
	out.write_base128(length());
	out.write_string(_eui);
}

} /* namespace horace */
