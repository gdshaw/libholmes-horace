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

namespace horace {

std::string attribute::name() const {
	switch (_type) {
	case attr_type_def:
		return "attr-def";
	case attr_type_code:
		return "attr-code";
	case attr_type_label:
		return "attr-label";
	case attr_type_format:
		return "attr-format";
	case attr_channel_def:
		return "channel-def";
	case attr_channel_num:
		return "channel-num";
	case attr_channel_label:
		return "channel-label";
	case ATTR_SOURCE:
		return "source";
	case ATTR_SEQNUM:
		return "seqnum";
	case ATTR_PACKET:
		return "packet";
	case ATTR_PACKET_LENGTH:
		return "packet_length";
	case ATTR_TIMESTAMP:
		return "timestamp";
	case ATTR_REPEAT:
		return "repeat";
	case ATTR_NETIF:
		return "netif";
	case ATTR_IFINDEX:
		return "ifindex";
	case ATTR_IFNAME:
		return "ifname";
	case ATTR_LINKTYPE:
		return "linktype";
	case ATTR_EUI:
		return "hwaddr";
	default:
		std::stringstream name_builder;
		name_builder << "attr" << _type;
		return name_builder.str();
	}
}

std::unique_ptr<attribute> attribute::parse(octet_reader& in,
	int type, size_t length) {

	switch (type) {
	case attr_type_def:
		return std::make_unique<compound_attribute>(type, length, in);
	case attr_type_code:
		return std::make_unique<unsigned_integer_attribute>(type, length, in);
	case attr_type_label:
		return std::make_unique<string_attribute>(type, length, in);
	case attr_type_format:
		return std::make_unique<unsigned_integer_attribute>(type, length, in);
	case attr_channel_def:
		return std::make_unique<compound_attribute>(type, length, in);
	case attr_channel_num:
		return std::make_unique<unsigned_integer_attribute>(type, length, in);
	case attr_channel_label:
		return std::make_unique<string_attribute>(type, length, in);
	case ATTR_SOURCE:
		return std::make_unique<string_attribute>(ATTR_SOURCE, length, in);
	case ATTR_SEQNUM:
		return std::make_unique<unsigned_integer_attribute>(ATTR_SEQNUM, length, in);
	case ATTR_PACKET:
		return std::make_unique<binary_attribute>(ATTR_PACKET, length, in);
	case ATTR_PACKET_LENGTH:
		return std::make_unique<unsigned_integer_attribute>(ATTR_PACKET_LENGTH, length, in);
	case ATTR_TIMESTAMP:
		return std::make_unique<timestamp_attribute>(ATTR_TIMESTAMP, length, in);
	case ATTR_REPEAT:
		return std::make_unique<unsigned_integer_attribute>(ATTR_REPEAT, length, in);
	case ATTR_NETIF:
		return std::make_unique<compound_attribute>(ATTR_NETIF, length, in);
	case ATTR_IFINDEX:
		return std::make_unique<unsigned_integer_attribute>(ATTR_IFINDEX, length, in);
	case ATTR_IFNAME:
		return std::make_unique<string_attribute>(ATTR_IFNAME, length, in);
	case ATTR_LINKTYPE:
		return std::make_unique<unsigned_integer_attribute>(ATTR_LINKTYPE, length, in);
	case ATTR_EUI:
		return std::make_unique<binary_attribute>(ATTR_EUI, length, in);
	default:
		return std::make_unique<unrecognised_attribute>(
			in, type, length);
	}
}

std::unique_ptr<attribute> attribute::parse(octet_reader& in) {
	int type = in.read_signed_base128();
	size_t length = in.read_unsigned_base128();
	return parse(in, type, length);
}

} /* namespace horace */
