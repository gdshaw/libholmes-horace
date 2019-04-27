// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_RECORD
#define LIBHOLMES_HORACE_PACKET_RECORD

#include "horace/packet_attribute.h"
#include "horace/absolute_timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

/** A class to represent a record containing a network packet. */
class packet_record:
	public record {
private:
	/** The packet attribute. */
	std::shared_ptr<packet_attribute> _packet_attr;

	/** The timestamp attribute. */
	std::shared_ptr<timestamp_attribute> _timestamp_attr;
public:
	/** Construct packet record with no attributes. */
	packet_record() = default;

	/** Move-construct from generic record.
	 * @param rec the record
	 */
	packet_record(record&& rec);

	/** Get the packet attribute.
	 * @return the packet attribute, or 0 if none
	 */
	const std::shared_ptr<packet_attribute> packet_attr() const {
		return _packet_attr;
	}

	/** Get the timestamp attribute.
	 * @return the timestamp attribute, or 0 if none
	 */
	const std::shared_ptr<timestamp_attribute> timestamp() const {
		return _timestamp_attr;
	}

	virtual int type() const {
		return REC_PACKET;
	}

	virtual std::string type_name() const {
		return "packet";
	}
};

} /* namespace horace */

#endif
