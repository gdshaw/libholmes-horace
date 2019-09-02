// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_RECORD
#define LIBHOLMES_HORACE_PACKET_RECORD

#include "horace/binary_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"

namespace horace {

/** A class to represent a record containing a network packet. */
class packet_record:
	public record {
private:
	/** An optional packet attribute. */
	const binary_ref_attribute* _packet_attr;

	/** An optional length attribute. */
	const unsigned_integer_attribute* _origlen_attr;

	/** An optional timestamp attribute. */
	const timestamp_attribute* _timestamp_attr;

	/** An optional repeat attribute. */
	const unsigned_integer_attribute* _repeat_attr;
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	explicit packet_record(record&& rec);

	/** Get the packet attribute.
	 * @return the packet attribute, or 0 if none
	 */
	const binary_ref_attribute* packet_attr() const {
		return _packet_attr;
	}

	/** Get the packet length attribute, if there is one.
	 * @return the packet length attribute, or 0 if none
	 */
	const unsigned_integer_attribute* origlen_attr() const {
		return _origlen_attr;
	}

	/** Get the timestamp attribute.
	 * @return the timestamp attribute, or 0 if none
	 */
	const timestamp_attribute* timestamp() const {
		return _timestamp_attr;
	}

	/** Get the repeat attribute.
	 * @return the repeat attribute, or 0 if none
	 */
	const unsigned_integer_attribute* repeat_attr() const {
		return _repeat_attr;
	}

	virtual std::string type_name() const {
		return "packet";
	}

	virtual void log(logger& log) const;
};

} /* namespace horace */

#endif
