// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SOCKET_DESCRIPTOR
#define LIBHOLMES_HORACE_SOCKET_DESCRIPTOR

#include "horace/file_descriptor.h"

namespace horace {

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
};

} /* namespace horace */

#endif
