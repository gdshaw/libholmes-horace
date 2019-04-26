// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sstream>

#include "horace/horace_error.h"
#include "horace/unrecognised_record.h"

namespace horace {

unrecognised_record::unrecognised_record(octet_reader& in,
	int type, size_t length):
	record(in, length),
	_type(type) {}

std::string unrecognised_record::type_name() const {
	std::ostringstream name;
	name << "rec" << _type;
	return name.str();
}

} /* namespace horace */
