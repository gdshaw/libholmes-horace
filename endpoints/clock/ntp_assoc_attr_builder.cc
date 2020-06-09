// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/session_builder.h"

#include "ntp_assoc_attr_builder.h"

namespace horace {

ntp_assoc_attr_builder::ntp_assoc_attr_builder(session_builder& session):
	_associd_attr(session.define_attribute("ntp_associd", type_unsigned_integer), 0),
	_status_attr(session.define_attribute("ntp_status", type_unsigned_integer), 0),
	_srchost_attr(session.define_attribute("ntp_srchost", type_string), ""),
	_srcaddr_attr(session.define_attribute("ntp_srcaddr", type_string), ""),
	_srcport_attr(session.define_attribute("ntp_srcport", type_unsigned_integer), 0),
	_stratum_attr(session.define_attribute("ntp_stratum", type_unsigned_integer), 0),
	_delay_attr(session.define_attribute("ntp_delay", type_signed_integer), 0),
	_offset_attr(session.define_attribute("ntp_offset", type_signed_integer), 0),
	_jitter_attr(session.define_attribute("ntp_jitter", type_signed_integer), 0),
	_assoc_attr(session.define_attribute("ntp_assoc", type_compound), attribute_list()) {}

void ntp_assoc_attr_builder::add_associd(uint16_t associd) {
	_attrs.append(_associd_attr = unsigned_integer_attribute(_associd_attr.attrid(), associd));
}

void ntp_assoc_attr_builder::add_status(uint16_t status) {
	_attrs.append(_status_attr = unsigned_integer_attribute(_status_attr.attrid(), status));
}

void ntp_assoc_attr_builder::add_srchost(const std::string& srchost) {
	_attrs.append(_srchost_attr = string_attribute(_srchost_attr.attrid(), srchost));
}

void ntp_assoc_attr_builder::add_srcaddr(const std::string& srcaddr) {
	_attrs.append(_srcaddr_attr = string_attribute(_srcaddr_attr.attrid(), srcaddr));
}

void ntp_assoc_attr_builder::add_srcport(uint16_t srcport) {
	_attrs.append(_srcport_attr = unsigned_integer_attribute(_srcport_attr.attrid(), srcport));
}

void ntp_assoc_attr_builder::add_stratum(unsigned int stratum) {
	_attrs.append(_stratum_attr = unsigned_integer_attribute(_stratum_attr.attrid(), stratum));
}

void ntp_assoc_attr_builder::add_delay(int64_t delay) {
	_attrs.append(_delay_attr = signed_integer_attribute(_delay_attr.attrid(), delay));
}

void ntp_assoc_attr_builder::add_offset(int64_t offset) {
	_attrs.append(_offset_attr = signed_integer_attribute(_offset_attr.attrid(), offset));
}

void ntp_assoc_attr_builder::add_jitter(int64_t jitter) {
	_attrs.append(_jitter_attr = signed_integer_attribute(_jitter_attr.attrid(), jitter));
}

const attribute& ntp_assoc_attr_builder::build() {
	_assoc_attr = compound_attribute(_assoc_attr.attrid(), std::move(_attrs));
	return _assoc_attr;
}

} /* namespace horace */
