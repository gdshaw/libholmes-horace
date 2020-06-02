// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <time.h>
#include <sys/timex.h>

#include "horace/terminate_flag.h"
#include "horace/log_message.h"
#include "horace/logger.h"
#include "horace/attribute_list.h"
#include "horace/string_attribute.h"
#include "horace/session_builder.h"

#include "clock_event_reader.h"
#include "clock_endpoint.h"
#include "local_timezone.h"
#include "ntp_client.h"

namespace horace {

class record;

clock_event_reader::clock_event_reader(const clock_endpoint& ep,
	session_builder& session):
	_ep(&ep),
	_first(true) {

	attribute_list attrs;
	_channel = session.define_channel("clock", std::move(attrs));
	_ntp_builder = std::make_unique<ntp_attr_builder>(session);
	_clock_builder = std::make_unique<clock_record_builder>(session, _channel);

	if (log->enabled(logger::log_notice)) {
		log_message msg(*log, logger::log_notice);
		msg << "monitoring clock (poll=" << _ep->poll() << ")";
	}
}

const record& clock_event_reader::read() {
	// The first record is generated immediately.
	// Subsequently, wait 60 seconds between records.
	if (_first) {
		_first = false;
	} else {
		terminating.millisleep(_ep->poll() * 1000);
	}

	// Add the NTP status.
	try {
		ntp_client client;
		client.update_peers();
		client.build(*_ntp_builder);
	} catch (std::exception& ex) {
		// No action
	}
	_clock_builder->add_ntp(_ntp_builder->build());

	// Add the timestamp.
	const struct timespec& ts = _clock_builder->add_ts();

	// Add the timezone offset and name.
	local_timezone ltz(ts.tv_sec);
	_clock_builder->add_tzoffset(ltz.tzoffset());
	_clock_builder->add_tzname(ltz.tzname());

	// Read the synchronisation state of the clock.
	struct timex tmx;
	tmx.modes = 0;
	int clock_state = ntp_adjtime(&tmx);
	_clock_builder->add_sync(clock_state != TIME_ERROR);

	return _clock_builder->build();
};

} /* namespace horace */
