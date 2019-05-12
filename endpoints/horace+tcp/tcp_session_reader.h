// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TCP_SESSION_READER
#define LIBHOLMES_HORACE_TCP_SESSION_READER

#include "horace/socket_descriptor.h"
#include "horace/file_octet_reader.h"
#include "horace/record.h"
#include "horace/session_reader.h"

namespace horace {

class tcp_endpoint;

/** A class for reading sessions from a file endpoint. */
class tcp_session_reader:
	public session_reader {
public:
	/** The source endpoint. */
	tcp_endpoint* _src_ep;

	/** The socket descriptor for reading. */
	socket_descriptor _fd;

	/** An octet reader for the socket descriptor. */
	file_octet_reader _fdor;
public:
	/** Construct TCP session reader.
	 * @param src_ep the source endpoint
	 * @param fd the socket descriptor
	 */
	tcp_session_reader(tcp_endpoint& src_ep,
		socket_descriptor&& fd);

	virtual std::unique_ptr<record> read();
};

} /* namespace horace */

#endif
