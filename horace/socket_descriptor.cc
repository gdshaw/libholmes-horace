// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sys/socket.h>

#include "libc_error.h"
#include "socket_descriptor.h"

namespace horace {

socket_descriptor::socket_descriptor(int domain, int type, int protocol):
	file_descriptor(::socket(domain, type, protocol)) {

	if (*this == -1) {
		throw libc_error();
	}
}

} /* namespace horace */
