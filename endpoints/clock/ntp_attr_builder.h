// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_ATTR_BUILDER
#define LIBHOLMES_HORACE_NTP_ATTR_BUILDER

#include <memory>

#include "horace/attribute_list.h"
#include "horace/compound_attribute.h"
#include "horace/string_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/signed_integer_attribute.h"
#include "horace/timestamp_attribute.h"

namespace horace {

class session_builder;

/** A class for building HORACE NTP attributes. */
class ntp_attr_builder {
private:
	/** The timestamp attribute. */
	timestamp_attribute _ts_attr;

	/** The srchost attribute. */
	string_attribute _srchost_attr;

	/** The srcadr attribute. */
	string_attribute _srcadr_attr;

	/** The srcport attribute. */
	unsigned_integer_attribute _srcport_attr;

	/** The status code attribute. */
	unsigned_integer_attribute _status_attr;

	/** The stratum attribute. */
	unsigned_integer_attribute _stratum_attr;

	/** The delay attribute. */
	signed_integer_attribute _delay_attr;

	/** The offset attribute. */
	signed_integer_attribute _offset_attr;

	/** The jitter attribute. */
	signed_integer_attribute _jitter_attr;

	/** The peer attribute. */
	compound_attribute _peer_attr;

	/** The attribute list under construction. */
	attribute_list _attrs;

	/** The NTP attribute. */
	compound_attribute _ntp_attr;
public:
	/** Construct empty NTP peer attribute builder.
	 * @param session the applicable session builder
	 */
	ntp_attr_builder(session_builder& session);

	/** Add peer.
	 * @param srchost the remote hostname
	 * @param srcadr the remote address
	 * @param srcport the remote port
	 * @param status the status code
	 * @param stratum the stratum number
	 * @param delay the peer delay, in nanoseconds
	 * @param offset the peer offset, in nanoseconds
	 * @param jitter the peer jitter, in nanoseconds
	 */
	void add_peer(const std::string& srchost, const std::string& srcadr,
		uint16_t srcport, uint16_t status, unsigned int stratum,
		int64_t delay, int64_t offset, int64_t jitter);

	/** Build NTP attribute. */
	const attribute& build();
};

} /* namespace horace */

#endif
