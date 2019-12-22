// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/log_message.h"
#include "horace/logger.h"
#include "horace/attribute_list.h"
#include "horace/string_attribute.h"
#include "horace/session_builder.h"

#include "unix_event_reader.h"
#include "unix_endpoint.h"

namespace horace {

class record;

unix_event_reader::unix_event_reader(const unix_endpoint& ep,
	session_builder& session):
	_ep(&ep) {

	attribute_list attrs;
	attrs.append(std::make_unique<string_attribute>(
		session.define_attribute("pathname", attrtype_string),
		_ep->pathname()));
	_channel = session.define_channel("unix", std::move(attrs));

	if (log->enabled(logger::log_notice)) {
		log_message msg(*log, logger::log_notice);
		msg << "capturing on " << _ep->pathname();
	}

	_builder = std::make_unique<packet_record_builder>(session, _channel);
	_sock = std::make_unique<unix_socket>(*_builder, _ep->snaplen());
	_sock->bind(_ep->pathname());
}

const record& unix_event_reader::read() {
	return _sock->read();
};

} /* namespace horace */
