// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/session_builder.h"

#include "ntp_attr_builder.h"

namespace horace {

ntp_attr_builder::ntp_attr_builder(session_builder& session):
	_ntp_attr(session.define_attribute("ntp", type_compound), attribute_list()) {}

void ntp_attr_builder::add_assoc_attr(const attribute& ntp_assoc_attr) {
	_attrs.append(ntp_assoc_attr.clone());
}

const attribute& ntp_attr_builder::build() {
	_ntp_attr = compound_attribute(_ntp_attr.attrid(), std::move(_attrs));
	return _ntp_attr;
}

} /* namespace horace */
