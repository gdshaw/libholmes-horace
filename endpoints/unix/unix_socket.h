// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNIX_SOCKET
#define LIBHOLMES_HORACE_UNIX_SOCKET

#include <string>

#include "horace/leap_second_corrector.h"
#include "horace/socket_descriptor.h"

namespace horace {

class record;
class packet_record_builder;

/** A class to represent a UNIX-domain socket. */
class unix_socket:
	public socket_descriptor {
private:
	/** The pathname to which this socket has been bound,
	 * or the empty string if none.
	 */
	std::string _pathname;

	/** The message snaplen. */
	size_t _snaplen;

	/** A buffer for receiving packet content. */
	std::unique_ptr<char[]> _buffer;

	/** An iovec structure for receiving packets. */
	struct iovec _iov[1];

	/** A msghdr structure for receiving packets. */
	struct msghdr _message;

	/** A leap second corrector for correcting timestamps. */
	leap_second_corrector _lsc;

	/** A builder for making packet records. */
	packet_record_builder* _builder;
public:
	/** Open UNIX-domain socket.
	 * @param builder a builder for making packet records
	 * @param snaplen the required link layer snaplen, in octets
	 */
	unix_socket(packet_record_builder& builder, size_t snaplen);

	/** Close UNIX-domain socket. */
	~unix_socket();

	/** Bind this socket to a pathname.
	 * @param pathname the required pathname
	 */
	void bind(const std::string& pathname);

	/** Read a message from this socket.
	 * This function will block unless a message is ready for reading.
	 *
	 * The reference returned by this function will remain usable until
	 * another message is read or the socket is destroyed. The same
	 * applies to the message content.
	 */
	const record& read();
};

} /* namespace horace */

#endif
