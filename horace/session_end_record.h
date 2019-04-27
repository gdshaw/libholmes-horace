// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_END_RECORD
#define LIBHOLMES_HORACE_SESSION_END_RECORD

#include "horace/timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

/** A class to represent an end of session record. */
class session_end_record:
	public record {
private:
	/** The timestamp attribute. */
	std::shared_ptr<timestamp_attribute> _timestamp_attr;
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	session_end_record(record&& rec);

	/** Get the timestamp attribute.
	 * @return the timestamp attribute
	 */
	const timestamp_attribute& timestamp() const {
		return *_timestamp_attr;
	}

	virtual std::string type_name() const {
		return "session-end";
	}
};

} /* namespace horace */

#endif
