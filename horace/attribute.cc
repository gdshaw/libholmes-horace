// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/attribute.h"
#include "horace/source_attribute.h"
#include "horace/seqnum_attribute.h"
#include "horace/unrecognised_attribute.h"

namespace horace {

std::unique_ptr<attribute> attribute::parse(octet_reader& in) {
	uint64_t type = in.read_base128();
	uint64_t length = in.read_base128();
	switch (type) {
	case ATTR_SOURCE:
		return std::make_unique<source_attribute>(in, length);
	case ATTR_SEQNUM:
		return std::make_unique<seqnum_attribute>(in, length);
	default:
		return std::make_unique<unrecognised_attribute>(
			in, type, length);
	}
}

} /* namespace horace */
