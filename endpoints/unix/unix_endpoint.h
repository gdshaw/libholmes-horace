// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNIX_ENDPOINT
#define LIBHOLMES_HORACE_UNIX_ENDPOINT

#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"

namespace horace {

/** An endpoint class to represent a UNIX-domain socket. */
class unix_endpoint:
	public endpoint,
	public event_reader_endpoint {
private:
	/** The snaplen, in octets. */
	long _snaplen;
public:
	/** Construct UNIX-domain endpoint.
	 * @param name the name of this endpoint
	 */
	explicit unix_endpoint(const std::string& name);

	/** Get the pathname.
	 * @return the pathname
	 */
	std::string pathname() const {
		return name().path();
	}

	/** Get the snaplen.
	 * @return the snaplen, in octets
	 */
	long snaplen() const {
		return _snaplen;
	}

	virtual std::unique_ptr<event_reader> make_event_reader(
		session_builder& session);
};

} /* namespace horace */

#endif
