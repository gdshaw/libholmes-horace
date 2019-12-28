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
	for (const auto& attr : _owned_attributes) {
		delete attr;
	}
}

attribute_list::attribute_list(const attribute_list& that) {
	for (const auto& attr : that._attributes) {
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
		for (const auto& attr : _owned_attributes) {
			delete attr;
		}
		_owned_attributes.clear();

		for (const auto& attr : that._attributes) {
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
		for (const auto& attr : _owned_attributes) {
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
		int attr_id = in.read_signed_base128(hdr_len);
		int attr_len = in.read_unsigned_base128(hdr_len);

		std::unique_ptr<attribute> attr =
			attribute::parse(session, attr_id, attr_len, in);
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
	for (const auto& attr : _attributes) {
		size_t attr_len = attr->length();
		len += octet_writer::signed_base128_length(attr->attrid());
		len += octet_writer::unsigned_base128_length(attr_len);
		len += attr_len;
	}
	return len;
}

bool attribute_list::contains(int attrid) const {
	for (const auto& attr : _attributes) {
		if (attr -> attrid() == attrid) {
			return true;
		}
	}
	return false;
}

const attribute& attribute_list::_find_one(int attrid) const {
	const attribute* found = 0;
	for (const auto& attr : _attributes) {
		if (attr -> attrid() == attrid) {
			if (found) {
				std::ostringstream msg;
				msg << "unexpected multiple attributes (ID="
					<< attrid << ")";
				throw horace_error(msg.str());
			}
			found = attr;
		}
	}
	if (!found) {
		std::ostringstream msg;
		msg << "expected attribute (ID=" << attrid << ")";
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
	for (const auto& attr : _attributes) {
		attr->write(out);
	}
}

bool operator==(const attribute_list& lhs, const attribute_list& rhs) {
	if (lhs._attributes.size() != rhs._attributes.size()) {
		return false;
	}
	for (auto i = lhs._attributes.begin(), j = rhs._attributes.begin();
		i != lhs._attributes.end(); ++i, ++j) {

		if (**i != **j) {
			return false;
		}
	}
	return true;
}

bool operator<=(const attribute_list& lhs, const attribute_list& rhs) {
	if (lhs._attributes.size() > rhs._attributes.size()) {
		return false;
	}
	for (auto i = lhs._attributes.begin(), j = rhs._attributes.begin();
		i != lhs._attributes.end(); ++i, ++j) {

		if (**i != **j) {
			return false;
		}
	}
	return true;
}

std::ostream& operator<<(std::ostream& out,
	const attribute_list& attrlist) {

	out << "(";
	for (const auto& attr : attrlist._attributes) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

} /* namespace horace */
