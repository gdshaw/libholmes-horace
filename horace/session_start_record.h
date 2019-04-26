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
	std::shared_ptr<source_attribute> _source_attr;

	/** The timestamp attribute. */
	std::shared_ptr<absolute_timestamp_attribute> _timestamp_attr;
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	session_start_record(record&& rec);

	/** Construct from required attributes.
	 * @param source_attr the source attribute
	 * @param timestamp_attr the timestamp attribute
	 */
	session_start_record(std::shared_ptr<source_attribute> source_attr,
		std::shared_ptr<absolute_timestamp_attribute> timestamp_attr);

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

	virtual int type() const {
		return REC_SESSION_START;
	}

	virtual std::string type_name() const {
		return "session-start";
	}
};

} /* namespace horace */

#endif
