// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>
#include <sstream>

#include "horace/horace_error.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/attribute.h"
#include "horace/attribute_list.h"

namespace horace {

attribute_list::~attribute_list() {
	for (const attribute* attr : _owned_attributes) {
		delete attr;
	}
}

attribute_list::attribute_list(const attribute_list& that) {
	for (auto attr : that._attributes) {
		auto attr_copy = attr->clone();
		_attributes.push_back(attr_copy.get());
		_owned_attributes.push_back(attr_copy.release());
	}
}

attribute_list::attribute_list(attribute_list&& that) {
	_attributes.swap(that._attributes);
	_owned_attributes.swap(that._owned_attributes);
}

attribute_list& attribute_list::operator=(const attribute_list& that) {
	if (this != &that) {
		_attributes.clear();
		for (const attribute* attr : _owned_attributes) {
			delete attr;
		}
		_owned_attributes.clear();

		for (auto attr : that._attributes) {
			auto attr_copy = attr->clone();
			_attributes.push_back(attr_copy.get());
			_owned_attributes.push_back(attr_copy.release());
		}
	}
	return *this;
}

attribute_list& attribute_list::operator=(attribute_list&& that) {
	if (this != &that) {
		_attributes.clear();
		for (const attribute* attr : _owned_attributes) {
			delete attr;
		}
		_owned_attributes.clear();

		_attributes.swap(that._attributes);
		_owned_attributes.swap(that._owned_attributes);
	}
	return *this;
}

attribute_list::attribute_list(session_context& session, size_t length,
	octet_reader& in) {

	size_t remaining = length;
	while (remaining) {
		size_t hdr_len = 0;
		int attr_type = in.read_signed_base128(hdr_len);
		int attr_len = in.read_unsigned_base128(hdr_len);

		std::unique_ptr<attribute> attr =
			attribute::parse(session, attr_type, attr_len, in);
		append(attr);

		size_t length = hdr_len + attr_len;
		if (length > remaining) {
			throw horace_error(
				"overrun while parsing attribute list");
		}
		remaining -= length;
	}
}

size_t attribute_list::length() const {
	size_t len = 0;
	for (auto&& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::signed_base128_length(attr->type());
		len += octet_writer::unsigned_base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

const attribute& attribute_list::_find_one(int type) const {
	const attribute* found = 0;
	for (auto&& attr : _attributes) {
		if (attr -> type() == type) {
			if (found) {
				std::ostringstream msg;
				msg << "unexpected multiple attributes of type "
					<< type;
				throw horace_error(msg.str());
			}
			found = attr;
		}
	}
	if (!found) {
		std::ostringstream msg;
		msg << "expected attribute of type " << type;
		throw horace_error(msg.str());
	}
	return *found;
}

attribute_list& attribute_list::append(
	std::unique_ptr<attribute>& attr) {

	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

attribute_list& attribute_list::append(
	std::unique_ptr<attribute>&& attr) {

	_attributes.push_back(attr.get());
	_owned_attributes.push_back(attr.release());
	return *this;
}

attribute_list& attribute_list::append(
	const attribute& attr) {

	_attributes.push_back(&attr);
	return *this;
}

void attribute_list::write(octet_writer& out) const {
	for (auto&& attr : _attributes) {
		attr->write(out);
	}
}

std::ostream& operator<<(std::ostream& out, const attribute_list& rec) {
	out << "(";
	for (auto&& attr : rec.attributes()) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

} /* namespace horace */
