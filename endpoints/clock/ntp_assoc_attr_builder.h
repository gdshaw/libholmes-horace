// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_ASSOC_ATTR_BUILDER
#define LIBHOLMES_HORACE_NTP_ASSOC_ATTR_BUILDER

#include "horace/attribute_list.h"
#include "horace/compound_attribute.h"
#include "horace/string_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/signed_integer_attribute.h"

namespace horace {

class session_builder;

/** A class for building HORACE NTP association attributes. */
class ntp_assoc_attr_builder {
private:
	/** The status code attribute. */
	unsigned_integer_attribute _status_attr;

	/** The srchost attribute. */
	string_attribute _srchost_attr;

	/** The srcaddr attribute. */
	string_attribute _srcaddr_attr;

	/** The srcport attribute. */
	unsigned_integer_attribute _srcport_attr;

	/** The stratum attribute. */
	unsigned_integer_attribute _stratum_attr;

	/** The delay attribute. */
	signed_integer_attribute _delay_attr;

	/** The offset attribute. */
	signed_integer_attribute _offset_attr;

	/** The jitter attribute. */
	signed_integer_attribute _jitter_attr;

	/** The attribute list under construction. */
	attribute_list _attrs;

	/** The NTP association attribute. */
	compound_attribute _assoc_attr;
public:
	/** Construct empty NTP association attribute builder.
	 * @param session the applicable session builder
	 */
	ntp_assoc_attr_builder(session_builder& session);

	/** Add status code.
	 * @param status the status code
	 */
	void add_status(uint16_t status);

	/** Add remote hostname.
	 * @param the remote hostname
	 */
	void add_srchost(const std::string& srchost);

	/** Add remote address.
	 * @param the remote address
	 */
	void add_srcaddr(const std::string& srchost);

	/** Add remote port number.
	 * @param the remote port number
	 */
	void add_srcport(uint16_t srcport);

	/** Add stratum number.
	 * @param the stratum number
	 */
	void add_stratum(unsigned int stratum);

	/** Add delay.
	 * @param the delay, in nanoseconds
	 */
	void add_delay(int64_t delay);

	/** Add offset.
	 * @param the offset, in nanoseconds
	 */
	void add_offset(int64_t offset);

	/** Add jitter.
	 * @param the jitter, in nanoseconds
	 */
	void add_jitter(int64_t jitter);

	/** Build NTP association attribute.
	 * Note that the result should be cloned if more than one
	 * attribute is to be built for the same record.
	 */
	const attribute& build();
};

} /* namespace horace */

#endif
