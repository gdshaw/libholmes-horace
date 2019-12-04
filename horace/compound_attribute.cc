// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>

#include "horace/octet_writer.h"
#include "horace/compound_attribute.h"

namespace horace {

compound_attribute::compound_attribute(int attrid, attribute_list&& attrlist):
	attribute(attrid),
	_attrlist(std::move(attrlist)) {}

compound_attribute::compound_attribute(session_context& session, int attrid,
	size_t length, octet_reader& in):
	attribute(attrid),
	_attrlist(session, length, in) {}

size_t compound_attribute::length() const {
	return _attrlist.length();
}

std::unique_ptr<attribute> compound_attribute::clone() const {
	attribute_list attrlist(_attrlist);
	return std::make_unique<compound_attribute>(
		attrid(), std::move(attrlist));
}

void compound_attribute::write(std::ostream& out) const {
	out << "attr" << attrid() << _attrlist;
}

void compound_attribute::write(octet_writer& out) const {
	out.write_signed_base128(attrid());
	out.write_unsigned_base128(length());
	_attrlist.write(out);
}

} /* namespace horace */
