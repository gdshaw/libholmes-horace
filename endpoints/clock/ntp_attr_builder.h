// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_ATTR_BUILDER
#define LIBHOLMES_HORACE_NTP_ATTR_BUILDER

#include "horace/attribute_list.h"
#include "horace/compound_attribute.h"

namespace horace {

class session_builder;

/** A class for building HORACE NTP attributes. */
class ntp_attr_builder {
private:
	/** The attribute list under construction. */
	attribute_list _attrs;

	/** The NTP attribute. */
	compound_attribute _ntp_attr;
public:
	/** Construct empty NTP attribute builder.
	 * @param session the applicable session builder
	 */
	ntp_attr_builder(session_builder& session);

	/** Add association attribute.
	 * @param assoc_attr the association attribute to be added
	 */
	void add_assoc_attr(const attribute& assoc_attr);

	/** Build NTP attribute. */
	const attribute& build();
};

} /* namespace horace */

#endif
