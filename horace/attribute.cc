// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/attribute.h"
#include "horace/source_attribute.h"
#include "horace/seqnum_attribute.h"
#include "horace/packet_attribute.h"
#include "horace/relative_timestamp_attribute.h"
#include "horace/posix_timestamp_attribute.h"
#include "horace/posix_timespec_attribute.h"
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
	case ATTR_PACKET:
		return std::make_unique<packet_attribute>(in, length);
	case ATTR_TIMESTAMP_S:
	case ATTR_TIMESTAMP_MS:
	case ATTR_TIMESTAMP_US:
	case ATTR_TIMESTAMP_NS:
		return std::make_unique<relative_timestamp_attribute>(
			in, type, length);
	case ATTR_POSIX_TIMESTAMP:
		return std::make_unique<posix_timestamp_attribute>(in, length);
	case ATTR_POSIX_TIMESPEC:
		return std::make_unique<posix_timespec_attribute>(in, length);
	default:
		return std::make_unique<unrecognised_attribute>(
			in, type, length);
	}
}

} /* namespace horace */
