// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ACK_RECORD
#define LIBHOLMES_HORACE_ACK_RECORD

#include "horace/record.h"
#include "horace/absolute_timestamp_attribute.h"
#include "horace/seqnum_attribute.h"

namespace horace {

class sync_record;

/** A class to represent an acknowledgement record. */
class ack_record:
	public record {
private:
	/** The timestamp attribute. */
	std::shared_ptr<absolute_timestamp_attribute> _timestamp_attr;

	/** The sequence number attribute. */
	std::shared_ptr<seqnum_attribute> _seqnum_attr;
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	explicit ack_record(record&& rec);

	/** Get the timestamp attribute.
	 * @return the timestamp attribute
	 */
	const absolute_timestamp_attribute& timestamp() const {
		return *_timestamp_attr;
	}

	/** Get the seqnum attribute.
	 * @return the seqnum attribute
	 */
	const seqnum_attribute& seqnum() const {
		return *_seqnum_attr;
	}

	virtual std::string type_name() const {
		return "ack";
	}

	/** Compare with a sync record.
	 * @return true if timestamp and seqnum match
	 */
	bool matches(const sync_record& rec) const;
};

} /* namespace horace */

#endif
