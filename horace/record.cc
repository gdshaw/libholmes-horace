// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/record.h"
#include "horace/unrecognised_record.h"

namespace horace {

std::unique_ptr<record> record::parse(octet_reader& in,
	int type, size_t length) {

	return std::make_unique<unrecognised_record>(in, type, length);
};

std::unique_ptr<record> record::parse(octet_reader& in) {
	int type = in.read_base128();
	size_t length = in.read_base128();
	return parse(in, type, length);
}

} /* namespace horace */
