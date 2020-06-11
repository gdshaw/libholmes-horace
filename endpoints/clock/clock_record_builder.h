// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_CLOCK_RECORD_BUILDER
#define LIBHOLMES_HORACE_CLOCK_RECORD_BUILDER

#include "horace/attribute_list.h"
#include "horace/signed_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/boolean_attribute.h"
#include "horace/record.h"

namespace horace {

class session_builder;

/** A class for building HORACE clock records.
 * A maximum of one attribute of each type may be added to each record.
 * Unlike other types of record builder, the timestamp attribute must be
 * explicitly added. This provides access to the timestamp value, which
 * can then be used to synchronously determine the timezone.
 */
class clock_record_builder {
private:
	/** The channel number. */
	int _channel;

	/** The timestamp attribute. */
	timestamp_attribute _ts_attr;

	/** The clock syncronised attribute. */
	boolean_attribute _sync_attr;

	/** The timezone offset attribute. */
	signed_integer_attribute _tzoffset_attr;

	/** The timezone abbreviation attribute. */
	string_attribute _tzabbrev_attr;

	/** The timezone name attribute. */
	string_attribute _tzname_attr;

	/** The attribute list under construction. */
	attribute_list _attrs;

	/** The most recently built record. */
	record _built;
public:
	/** Construct empty clock record builder.
	 * @param session the applicable session builder
	 * @param channel the applicable channel number
	 */
	clock_record_builder(session_builder& session, int channel);

	/** Add timestamp.
	 * @return the time recorded in the timestamp
	 */
	const struct timespec& add_ts();

	/** Add clock sync state.
	 * @param sync the clock sync state to be added
	 */
	void add_sync(bool sync);

	/** Add timezone offset.
	 * This is the offset from UTC to local time.
	 * @param tzoffset the timezone offset, in seconds
	 */
	void add_tzoffset(long tzoffset);

	/** Add timezone abbreviation.
	 * @param tzabbrev the timezone abbreviation
	 */
	void add_tzabbrev(const std::string& tzabbrev);

	/** Add timezone name.
	 * @param tzname the timezone name
	 */
	void add_tzname(const std::string& tzname);

	/** Add NTP attribute.
	 * @param ntp_attr the NTP attribute to be added
	 */
	void add_ntp(const attribute& ntp_attr);

	/** Build clock record.
	 */
	const record& build();
};

} /* namespace horace */

#endif
