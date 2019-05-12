// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TCP_SESSION_WRITER
#define LIBHOLMES_HORACE_TCP_SESSION_WRITER

#include "horace/socket_descriptor.h"
#include "horace/file_octet_writer.h"
#include "horace/session_writer.h"

namespace horace {

class tcp_endpoint;

/** A class for writing sessions to a TCP connection. */
class tcp_session_writer:
	public session_writer {
private:
	/** The destination endpoint. */
	tcp_endpoint* _dst_ep;

	/** A socket descriptor for the connection. */
	socket_descriptor _fd;

	/** An octet writer for the connection. */
	file_octet_writer _fdow;

	/** Open a connection. */
	void _open();
protected:
	virtual void handle_session_start(const session_start_record& srec);
	virtual void handle_session_end(const session_end_record& erec);
	virtual void handle_event(const record& rec);
public:
	/** Construct TCP session writer.
	 * @param dst_ep the destination endpoint
	 * @param source_id the required source ID
	 */
	tcp_session_writer(tcp_endpoint& dst_ep,
		const std::string& source_id);
};

} /* namespace horace */

#endif
