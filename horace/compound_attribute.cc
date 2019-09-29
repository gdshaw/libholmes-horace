// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "compound_attribute.h"

namespace horace {

compound_attribute::compound_attribute(int type, attribute_list&& attrlist):
	attribute(type),
	_attrlist(std::move(attrlist)) {}

compound_attribute::compound_attribute(session_context& session, int type,
	size_t length, octet_reader& in):
	attribute(type),
	_attrlist(session, in, length) {}

size_t compound_attribute::length() const {
	return _attrlist.length();
}

std::unique_ptr<attribute> compound_attribute::clone() const {
	attribute_list attrlist(_attrlist);
	return std::make_unique<compound_attribute>(
		type(), std::move(attrlist));
}

void compound_attribute::write(std::ostream& out) const {
	out << "attr" << type() << _attrlist;
}

void compound_attribute::write(octet_writer& out) const {
	out.write_signed_base128(type());
	out.write_unsigned_base128(length());
	_attrlist.write(out);
}

} /* namespace horace */
