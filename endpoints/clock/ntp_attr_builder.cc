// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/session_builder.h"

#include "ntp_attr_builder.h"

namespace horace {

ntp_attr_builder::ntp_attr_builder(session_builder& session):
	_ts_attr(attrid_ts, 0, 0),
	_srchost_attr(session.define_attribute("ntp_srchost", attrfmt_string), ""),
	_srcadr_attr(session.define_attribute("ntp_srcadr", attrfmt_string), ""),
	_srcport_attr(session.define_attribute("ntp_srcport", attrfmt_unsigned_integer), 0),
	_status_attr(session.define_attribute("ntp_status", attrfmt_unsigned_integer), 0),
	_stratum_attr(session.define_attribute("ntp_stratum", attrfmt_unsigned_integer), 0),
	_delay_attr(session.define_attribute("ntp_delay", attrfmt_signed_integer), 0),
	_offset_attr(session.define_attribute("ntp_offset", attrfmt_signed_integer), 0),
	_jitter_attr(session.define_attribute("ntp_jitter", attrfmt_signed_integer), 0),
	_peer_attr(session.define_attribute("ntp_peer", attrfmt_compound), attribute_list()),
	_ntp_attr(session.define_attribute("ntp", attrfmt_compound), attribute_list()) {}

void ntp_attr_builder::add_peer(const std::string& srchost, const std::string& srcadr,
	uint16_t srcport, uint16_t status, unsigned int stratum,
	int64_t delay, int64_t offset, int64_t jitter) {

	attribute_list attrs;
	if (!srchost.empty()) {
		attrs.append(_srchost_attr = string_attribute(_srchost_attr.attrid(), srchost));
	}
	if (!srcadr.empty()) {
		attrs.append(_srcadr_attr = string_attribute(_srcadr_attr.attrid(), srcadr));
	}
	if (srcport != 123) {
		attrs.append(_srcport_attr = unsigned_integer_attribute(_srcport_attr.attrid(), srcport));
	}
	attrs.append(_status_attr = unsigned_integer_attribute(_status_attr.attrid(), status));
	attrs.append(_stratum_attr = unsigned_integer_attribute(_stratum_attr.attrid(), stratum));
	attrs.append(_delay_attr = signed_integer_attribute(_delay_attr.attrid(), delay));
	attrs.append(_offset_attr = signed_integer_attribute(_offset_attr.attrid(), offset));
	attrs.append(_jitter_attr = signed_integer_attribute(_jitter_attr.attrid(), jitter));

	_peer_attr = compound_attribute(_peer_attr.attrid(), attribute_list(attrs));
	_attrs.append(_peer_attr.clone());
}

const attribute& ntp_attr_builder::build() {
	_attrs.append(_ts_attr = timestamp_attribute(_ts_attr.attrid()));
	_ntp_attr = compound_attribute(_ntp_attr.attrid(), std::move(_attrs));
	return _ntp_attr;
}

} /* namespace horace */
