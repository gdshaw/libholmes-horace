// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TCP_ENDPOINT
#define LIBHOLMES_HORACE_TCP_ENDPOINT

#include <string>

#include "horace/endpoint.h"
#include "horace/session_listener_endpoint.h"
#include "horace/session_writer_endpoint.h"

namespace horace {

/** An endpoint class for making and receiving connections via TCP. */
class tcp_endpoint:
	public endpoint,
	public session_listener_endpoint,
	public session_writer_endpoint {
private:
	/** The hostname. */
	std::string _hostname;

	/** The portname. */
	std::string _portname;

	/** The retry interval, in seconds. */
	long _retry;
public:
	/** Construct TCP endpoint.
	 * @param name the name of this endpoint
	 */
	explicit tcp_endpoint(const std::string& name);

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

	/** Get the retry interval.
	 * @return the retry interval, in seconds
	 */
	long retry() const {
		return _retry;
	}

	virtual std::unique_ptr<session_listener> make_session_listener();
	virtual std::unique_ptr<session_writer> make_session_writer(
		const std::string& srcid);
};

} /* namespace horace */

#endif
