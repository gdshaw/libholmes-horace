// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_CLOCK_EVENT_READER
#define LIBHOLMES_HORACE_CLOCK_EVENT_READER

#include "horace/boolean_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"
#include "horace/event_reader.h"

#include "ntp_attr_builder.h"
#include "clock_record_builder.h"

namespace horace {

class session_builder;
class clock_endpoint;

/** A class for monitoring the system clock. */
class clock_event_reader:
	public event_reader {
private:
	/** The clock endpoint. */
	const clock_endpoint* _ep;

	/** The channel number to use for monitored events. */
	int _channel;

	/** True if first record not yet generated, otherwise false. */
	bool _first;

	/** A builder for NTP attributes. */
	std::unique_ptr<ntp_attr_builder> _ntp_builder;

	/** A builder for clock records. */
	std::unique_ptr<clock_record_builder> _clock_builder;
public:
	/** Construct clock event reader.
	 * @param ep the endpoint to read from
	 */
	explicit clock_event_reader(const clock_endpoint& ep,
		session_builder& builder);

	virtual const record& read();
};

} /* namespace horace */

#endif
