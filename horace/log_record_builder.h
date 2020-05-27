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

	/** The syslog version attribute. */
	unsigned_integer_attribute _version_attr;

	/** The syslog timestamp attribute. */
	string_attribute _timestamp_attr;

	/** The syslog hostname attribute. */
	string_attribute _hostname_attr;

	/** The syslog appname attribute. */
	string_attribute _appname_attr;

	/** The syslog procid attribute. */
	string_attribute _procid_attr;

	/** The syslog msgid attribute. */
	string_attribute _msgid_attr;

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

	/** Add syslog severity.
	 * @param severity the severity to be added
	 */
	void add_severity(unsigned int severity);

	/** Add syslog facility.
	 * @param facility the facility to be added
	 */
	void add_facility(unsigned int facility);

	/** Add syslog version.
	 * @param version the version to be added
	 */
	void add_version(unsigned int version);

	/** Add syslog timestamp.
	 * @param timestamp the timestamp to be added
	 */
	void add_timestamp(const std::string& timestamp);

	/** Add syslog hostname.
	 * @param hostname the hostname to be added
	 */
	void add_hostname(const std::string& hostname);

	/** Add syslog appname.
	 * @param appname the appname to be added
	 */
	void add_appname(const std::string& appname);

	/** Add syslog procid.
	 * @param procid the procid to be added
	 */
	void add_procid(const std::string& procid);

	/** Add syslog msgid.
	 * @param msgid the msgid to be added
	 */
	void add_msgid(const std::string& msgid);

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
