// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sstream>

#include "horace/attribute.h"
#include "horace/binary_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/unrecognised_attribute.h"
#include "horace/session_context.h"

namespace horace {

std::unique_ptr<attribute> attribute::parse(session_context& session,
	int type, size_t length, octet_reader& in) {

	int format = session.get_attr_format(type);
	switch (format) {
	case attr_format_compound:
		return std::make_unique<compound_attribute>(session, type, length, in);
	case attr_format_unsigned_integer:
		return std::make_unique<unsigned_integer_attribute>(type, length, in);
	case attr_format_binary:
		return std::make_unique<binary_attribute>(type, length, in);
	case attr_format_string:
		return std::make_unique<string_attribute>(type, length, in);
	case attr_format_timestamp:
		return std::make_unique<timestamp_attribute>(type, length, in);
	default:
		return std::make_unique<unrecognised_attribute>(
			type, length, in);
	}
}

std::unique_ptr<attribute> attribute::parse(session_context& session,
	octet_reader& in) {

	int type = in.read_signed_base128();
	size_t length = in.read_unsigned_base128();
	return parse(session, type, length, in);
}

} /* namespace horace */
