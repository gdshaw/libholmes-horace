// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_START_RECORD
#define LIBHOLMES_HORACE_SESSION_START_RECORD

#include "horace/source_attribute.h"
#include "horace/absolute_timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

/** A class to represent a start of session record. */
class session_start_record:
	public record {
private:
	/** The source attribute. */
	const source_attribute* _source_attr;

	/** The timestamp attribute. */
	const absolute_timestamp_attribute* _timestamp_attr;
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	explicit session_start_record(record&& rec);

	/** Get the source attribute.
	 * @return the source attribute
	 */
	const source_attribute& source_attr() const {
		return *_source_attr;
	}

	/** Get the timestamp attribute.
	 * @return the timestamp attribute
	 */
	const absolute_timestamp_attribute& timestamp() const {
		return *_timestamp_attr;
	}

	virtual std::string type_name() const {
		return "session-start";
	}

	virtual void log(logger& log) const;

	/** Compare with another start of session record.
	 * @return true if source and timestamp match
	 */
	bool matches(const session_start_record& rec) const;
};

} /* namespace horace */

#endif
