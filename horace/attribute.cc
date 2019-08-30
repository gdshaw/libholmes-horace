// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/attribute.h"
#include "horace/string_attribute.h"
#include "horace/packet_attribute.h"
#include "horace/relative_timestamp_attribute.h"
#include "horace/posix_timestamp_attribute.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/netif_attribute.h"
#include "horace/eui_attribute.h"
#include "horace/unrecognised_attribute.h"

namespace horace {

std::unique_ptr<attribute> attribute::parse(octet_reader& in,
	int type, size_t length) {

	switch (type) {
	case ATTR_SOURCE:
		return std::make_unique<string_attribute>(ATTR_SOURCE, length, in);
	case ATTR_SEQNUM:
		return std::make_unique<unsigned_integer_attribute>(ATTR_SEQNUM, length, in);
	case ATTR_PACKET:
		return std::make_unique<packet_attribute>(in, length);
	case ATTR_PACKET_LENGTH:
		return std::make_unique<unsigned_integer_attribute>(ATTR_PACKET_LENGTH, length, in);
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
	case ATTR_REPEAT:
		return std::make_unique<unsigned_integer_attribute>(ATTR_REPEAT, length, in);
	case ATTR_NETIF:
		return std::make_unique<netif_attribute>(in, length);
	case ATTR_IFINDEX:
		return std::make_unique<unsigned_integer_attribute>(ATTR_IFINDEX, length, in);
	case ATTR_IFNAME:
		return std::make_unique<string_attribute>(ATTR_IFNAME, length, in);
	case ATTR_LINKTYPE:
		return std::make_unique<unsigned_integer_attribute>(ATTR_LINKTYPE, length, in);
	case ATTR_EUI:
		return std::make_unique<eui_attribute>(in, length);
	}
}

std::unique_ptr<attribute> attribute::parse(octet_reader& in) {
	int type = in.read_signed_base128();
	size_t length = in.read_unsigned_base128();
	return parse(in, type, length);
}

} /* namespace horace */
