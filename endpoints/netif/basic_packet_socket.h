// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_BASIC_PACKET_SOCKET
#define LIBHOLMES_HORACE_BASIC_PACKET_SOCKET

#include "horace/socket_descriptor.h"

#include "interface.h"

namespace horace {

class record;

/** An abstract base class to represent an AF_PACKET socket.
 * This class does not by itself provide the means to read packets from the
 * socket, since there are a variety of methods which can be used to achieve
 * that (using recvfrom/recvmsg, or via a ring buffer).
 */
class basic_packet_socket:
	public socket_descriptor {
public:
	/** Open basic packet socket. */
	basic_packet_socket();

	/** Read a packet from this socket.
	 * This function will block unless a packet is ready for reading.
	 *
	 * The reference returned by this function will remain usable until
	 * another packet is read or the socket is destroyed. The same
	 * applies to the packet content.
	 */
	virtual const record& read() = 0;

	/** Bind this socket to a given interface.
	 * @param iface the interface
	 */
	void bind(const interface& iface);

	/** Set promiscuous mode for a given interface.
	 * @param iface the interface
	 */
	void set_promiscuous(const interface& iface);
};

} /* namespace horace */

#endif
