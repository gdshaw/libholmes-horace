// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SOCKET_DESCRIPTOR
#define LIBHOLMES_HORACE_SOCKET_DESCRIPTOR

#include <sys/socket.h>

#include "horace/file_descriptor.h"

namespace horace {

class filter;

/** A class to represent a socket descriptor. */
class socket_descriptor:
	public file_descriptor {
public:
	/** Create invalid socket descriptor. */
	socket_descriptor() = default;

	/** Wrap raw socket descriptor.
	 * The resulting class instance takes ownership of the
	 * raw socket descriptor passed.
	 * @param fd the raw socket descriptor
	 */
	explicit socket_descriptor(int fd):
		file_descriptor(fd) {}

	/** Open new socket descriptor.
	 * @param domain the required address family
	 * @param type the required socket type
	 * @param protocol the required protocol
	 */
	socket_descriptor(int domain, int type, int protocol);

	/** Bind this socket descriptor to an address.
	 * @param addr the address
	 * @param addrlen the length of the address
	 */
	void bind(const struct sockaddr* addr, socklen_t addrlen);

	/** Bind this socket descriptor to an address.
	 * @param addr the address
	 */
	template<class T>
	void bind(const T& addr) {
		bind(reinterpret_cast<const struct sockaddr*>(&addr),
			sizeof(addr));
	}

	/** Listen on this socket.
	 * @param the required queue length
	 */
	void listen(int backlog = SOMAXCONN);

	/** Accept a connection.
	 * If a connection is not immediately available then this
	 * function will block, even if the socket is non-blocking.
	 * If it detects a request to terminate then it will throw
	 * a terminate_exception.
	 * @param addr a buffer to receive the remote address
	 * @param addrlen the size of the buffer
	 * @return a socket descriptor for the accepted connection
	 */
	socket_descriptor accept(struct sockaddr* addr, socklen_t addrlen);

	/** Connect this socket descriptor to an address.
	 * @param addr the address
	 * @param addrlen the length of the address
	 */
	void connect(const struct sockaddr* addr, socklen_t addrlen);

	/** Connect this socket descriptor to an address.
	 * @param addr the address
	 */
	template<class T>
	void connect(const T& addr) {
		bind(reinterpret_cast<const struct sockaddr*>(&addr),
			sizeof(addr));
	}

	/** Receive a message from this socket.
	 * This function will block if a message is not immediately
	 * available, unless there is no prospect of any further messages
	 * being received.
	 * @param message a msghdr structure for receiving the message
	 * @param flags a bitwise combination of zero or more MSG_ flags
	 * @return the received message length, or 0 if none
	 */
	size_t recvmsg(struct msghdr* message, int flags);

	/** Set socket option.
	 * @param level the socket option level
	 * @param optname the socket option name
	 * @param optval the required value
	 * @param optlen the size of the value
	 */
	void setsockopt(int level, int optname, const void* optval,
		socklen_t optlen);

	/** Set socket option.
	 * @param level the socket option level
	 * @param optname the socket option name
	 * @param optval the required value
	 */
	template<class T>
	void setsockopt(int level, int optname, const T& optval) {
		setsockopt(level, optname, &optval, sizeof(optval));
	}

	/** Attach a BPF filter to this socket descriptor.
	 */
	void attach(const filter& filt);
};

} /* namespace horace */

#endif
