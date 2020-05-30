// This file is part of libholmes.
// Copyright 2019-20 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sstream>

#include "horace/octet_reader.h"
#include "horace/attribute.h"
#include "horace/compound_attribute.h"
#include "horace/signed_integer_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/binary_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/boolean_attribute.h"
#include "horace/unrecognised_attribute.h"
#include "horace/session_context.h"

namespace horace {

std::unique_ptr<attribute> attribute::parse(session_context& session,
	int attrid, size_t length, octet_reader& in) {

	int attrfmt = session.get_attr_fmt(attrid);
	switch (attrfmt) {
	case attrfmt_compound:
		return std::make_unique<compound_attribute>(session, attrid, length, in);
	case attrfmt_unsigned_integer:
		return std::make_unique<unsigned_integer_attribute>(attrid, length, in);
	case attrfmt_signed_integer:
		return std::make_unique<signed_integer_attribute>(attrid, length, in);
	case attrfmt_binary:
		return std::make_unique<binary_attribute>(attrid, length, in);
	case attrfmt_string:
		return std::make_unique<string_attribute>(attrid, length, in);
	case attrfmt_timestamp:
		return std::make_unique<timestamp_attribute>(attrid, length, in);
	case attrfmt_boolean:
		return std::make_unique<boolean_attribute>(attrid, length, in);
	default:
		return std::make_unique<unrecognised_attribute>(
			attrid, length, in);
	}
}

std::unique_ptr<attribute> attribute::parse(session_context& session,
	octet_reader& in) {

	int attrid = in.read_signed_base128();
	size_t length = in.read_unsigned_base128();
	return parse(session, attrid, length, in);
}

} /* namespace horace */
