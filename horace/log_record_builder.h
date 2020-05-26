// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LOG_RECORD_BUILDER
#define LIBHOLMES_HORACE_LOG_RECORD_BUILDER

#include "horace/attribute_list.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

class session_builder;

/** A class for building HORACE log records.
 * A maximum of one attribute of each type may be added to each record.
 */
class log_record_builder {
private:
	/** The channel number. */
	int _channel;

	/** The timestamp attribute. */
	timestamp_attribute _ts_attr;

	/** The syslog severity attribute. */
	unsigned_integer_attribute _severity_attr;

	/** The syslog facility attribute. */
	unsigned_integer_attribute _facility_attr;

	/** The message attribute. */
	string_attribute _message_attr;

	/** The attribute list under construction. */
	attribute_list _attrs;

	/** The most recently built record. */
	record _built;
public:
	/** Construct empty log record builder.
	 * @param session the applicable session builder
	 * @param channel the applicable channel number
	 */
	log_record_builder(session_builder& session, int channel);

	/** Add Syslog severity.
	 * @param severity the severity to be added
	 */
	void add_severity(unsigned int severity);

	/** Add Syslog facility.
	 * @param facility the facility to be added
	 */
	void add_facility(unsigned int facility);

	/** Add message.
	 * @param message the message to be added
	 */
	void add_message(const std::string& message);

	/** Build log record.
	 */
	const record& build();
};

} /* namespace horace */

#endif
