// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_CLOCK_RECORD_BUILDER
#define LIBHOLMES_HORACE_CLOCK_RECORD_BUILDER

#include "horace/attribute_list.h"
#include "horace/timestamp_attribute.h"
#include "horace/boolean_attribute.h"
#include "horace/record.h"

namespace horace {

class session_builder;

/** A class for building HORACE clock records.
 * A maximum of one attribute of each type may be added to each record.
 */
class clock_record_builder {
private:
	/** The channel number. */
	int _channel;

	/** The timestamp attribute. */
	timestamp_attribute _ts_attr;

	/** The clock syncronised attribute. */
	boolean_attribute _sync_attr;

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

	/** Add clock sync state.
	 * @param sync the clock sync state to be added
	 */
	void add_sync(bool sync);

	/** Build clock record.
	 */
	const record& build();
};

} /* namespace horace */

#endif
