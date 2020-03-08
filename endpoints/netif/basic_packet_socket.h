// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_BASIC_PACKET_SOCKET
#define LIBHOLMES_HORACE_BASIC_PACKET_SOCKET

#include <mutex>

#include "horace/socket_descriptor.h"
#include "horace/leap_second_monitor.h"
#include "horace/leap_second_corrector.h"

namespace horace {

class record;

class interface;

/** An abstract base class to represent an AF_PACKET socket.
 * This class does not by itself provide the means to read packets from the
 * socket, since there are a variety of methods which can be used to achieve
 * that (using recvfrom/recvmsg, or via a ring buffer).
 */
class basic_packet_socket:
	public socket_descriptor,
	public leap_second_monitor::measurable {
private:
	/** A mutex protecting _packets and _drops. */
	std::mutex _mutex;

	/** The total number of packets received.
	 * This included both packets which have been read, and packets in
	 * the buffer which are waiting to be read.
	 */
	uint64_t _packets;

	/** The number of dropped packets since last checked. */
	unsigned int _drops;

	/** Update _packets and _drops. */
	void _update_stats();
protected:
	/** A leap second corrector for this socket. */
	leap_second_corrector lsc;
public:
	/** Open basic packet socket. */
	basic_packet_socket();

	/** Close basic packet socket. */
	~basic_packet_socket();

	virtual void measure();

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

	/** Get the number of dropped packets.
	 * @return the number of dropped packets since previous call
	 */
	unsigned int drops();

	/** Get the method name for this socket.
	 * @return the method name
	 */
	virtual const std::string& method() const = 0;
};

} /* namespace horace */

#endif
