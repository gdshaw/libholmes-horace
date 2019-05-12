// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TCP_SESSION_LISTENER
#define LIBHOLMES_HORACE_TCP_SESSION_LISTENER

#include "horace/socket_descriptor.h"
#include "horace/session_listener.h"

namespace horace {

class tcp_endpoint;

/** A class for listening for sessions via TCP. */
class tcp_session_listener:
	public session_listener {
public:
	/** The source endpoint. */
	tcp_endpoint* _src_ep;

	/** The socket on which to listen. */
	socket_descriptor _fd;
public:
	/** Construct filestore session listener.
	 * @param src_ep the source endpoint
	 */
	tcp_session_listener(tcp_endpoint& src_ep);

	virtual std::unique_ptr<session_reader> accept();
};

} /* namespace horace */

#endif
