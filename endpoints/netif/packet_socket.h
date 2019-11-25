// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_SOCKET
#define LIBHOLMES_HORACE_PACKET_SOCKET

#include <memory>
#include <cstddef>

#include <sys/uio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>

#include "basic_packet_socket.h"

namespace horace {

class packet_record_builder;

/** A class for reading traffic from an AF_PACKET socket. */
class packet_socket:
	public basic_packet_socket {
private:
	/** The link layer snaplen. */
	size_t _snaplen;

	/** A buffer for receiving packet content. */
	std::unique_ptr<char[]> _buffer;

	/** A buffer for receiving control messages. */
	std::unique_ptr<char[]> _control;

	/** A structure for receiving packet source addresses. */
	struct sockaddr_ll _src_addr;

	/** An iovec structure for receiving packets. */
	struct iovec _iov[1];

	/** A msghdr structure for receiving packets. */
	struct msghdr _message;

	/** The most recently observed dropped packet counter. */
	uint32_t _drop_count;

	/** A builder for making packet records. */
	packet_record_builder* _builder;
public:
	/** Open packet socket.
	 * @param builder a builder for making packet records
	 * @param snaplen the required link layer snaplen, in octets
	 * @param buffer_size the required ring buffer size, in octets
	 */
	packet_socket(packet_record_builder& builder, size_t snaplen, size_t buffer_size);

	virtual const record& read();
};

} /* namespace horace */

#endif

