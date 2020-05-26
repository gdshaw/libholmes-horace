// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SYSLOG_UDP_ENDPOINT
#define LIBHOLMES_HORACE_SYSLOG_UDP_ENDPOINT

#include <string>

#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"

namespace horace {

/** An endpoint class to represent a Syslog UDP socket. */
class syslog_udp_endpoint:
	public endpoint,
	public event_reader_endpoint {
private:
	/** The hostname. */
	std::string _hostname;

	/** The portname. */
	std::string _portname;
public:
	/** Construct Syslog over UDP endpoint.
	 * @param name the name of this endpoint
	 */
	explicit syslog_udp_endpoint(const std::string& name);

	virtual std::unique_ptr<event_reader> make_event_reader(
		session_builder& session);

	/** Get the hostname.
	 * @return the hostname
	 */
	const std::string& hostname() const {
		return _hostname;
	}

	/** Get the portname.
	 * @return the portname
	 */
	const std::string& portname() const {
		return _portname;
	}
};

} /* namespace horace */

#endif
