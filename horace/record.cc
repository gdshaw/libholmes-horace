// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sstream>
#include <iomanip>

#include "horace/horace_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/record.h"
#include "horace/session_context.h"

namespace horace {

record::record(session_context& session, octet_reader& in) {
	_channel = in.read_signed_base128();
	size_t remaining = in.read_unsigned_base128();
	while (remaining) {
		size_t hdr_len = 0;
		int attr_id = in.read_signed_base128(hdr_len);
		int attr_len = in.read_unsigned_base128(hdr_len);

		std::unique_ptr<attribute> attr =
			attribute::parse(session, attr_id, attr_len, in);
		switch (attr->attrid()) {
		case attrid_attr_def:
			session.handle_attr_def(
				dynamic_cast<compound_attribute&>(*attr));
			break;
		case attrid_channel_def:
			session.handle_channel_def(
				dynamic_cast<compound_attribute&>(*attr));
			break;
		default:
			// no action
			break;
		}
		_attributes.append(attr);

		size_t length = hdr_len + attr_len;
		if (length > remaining) {
			throw horace_error(
				"attribute extends beyond length of record");
		}
		remaining -= length;
	}
}

void record::write(octet_writer& out) const {
	out.write_signed_base128(_channel);
	out.write_unsigned_base128(_attributes.length());
	_attributes.write(out);
}

void record::log(logger& log) const {
	if (log.enabled(logger::log_info)) {
		log_message msg(log, logger::log_info);
		switch (_channel) {
		case channel_error:
			msg << "error record";
			break;
		case channel_session:
			msg << "session record";
			break;
		case channel_sync:
			msg << "sync record";
			break;
		default:
			if (_channel >= 0) {
				msg << "event";
			} else {
				msg << "unrecognised";
			}
			msg << " record for channel " << _channel;
		}
	}
}

bool operator==(const record& lhs, const record& rhs) {
	return ((lhs._channel == rhs._channel) &&
		(lhs._attributes == rhs._attributes));
}

std::ostream& operator<<(std::ostream& out, const record& rec) {
	out << "rec" << rec.channel_number() << "(";
	for (const auto& attr : rec.attributes()) {
		out << std::endl;
		out << " " << *attr;
	}
	out << ")" << std::endl;
}

} /* namespace horace */
