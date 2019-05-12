// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ADDRESS_INFO
#define LIBHOLMES_HORACE_ADDRESS_INFO

#include <string>

#include <netdb.h>

#include "horace/socket_descriptor.h"

namespace horace {

/** A class for looking up hostnames and portnames.
 * The current implementation uses only the first result.
 */
class address_info {
private:
	/** The retrieved address information. */
	struct addrinfo* _res;
public:
	/** Look up address information.
	 * @param hostname the hostname
	 * @param portname the portname
	 * @param server true if address for server, false for client
	 */
	address_info(const std::string& hostname,
		const std::string& portname, bool server);

	/** Destroy retrieved address information. */
	~address_info();

	/** Make a socket from the result.
	 * @return the socket
	 */
	socket_descriptor make_socket() const;

	/** Bind a socket to the result.
	 * @param fd the socket descriptor to bind
	 */
	void bind(socket_descriptor& fd) const;

	/** Connect a socket to the result.
	 * @param fd the socket descriptor to connect
	 */
	void connect(socket_descriptor& fd) const;
};

} /* namespace horace */

#endif
