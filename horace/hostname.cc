// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <memory>
#include <cstring>

#include <unistd.h>
#include <netdb.h>

#include "horace/hostname.h"

namespace horace {

hostname::hostname() {
	// Begin with a buffer length of 64 bytes, which ought to be
	// sufficient for most reasonable hostnames.
	size_t hostname_len = 64;
	std::unique_ptr<char[]> hostname;

	// Loop until an untruncated hostname has been read.
	while (true) {
		// Allocate a buffer of the current proposed length.
		hostname = std::make_unique<char[]>(hostname_len);

		// Initialise the last byte of the buffer to help
		// detect truncation.
		hostname[hostname_len - 1] = 0;

		// Attempt to read the hostname into the buffer.
		if (gethostname(hostname.get(), hostname_len - 1) == 0) {
			// Attempt to detect truncation. It is not possible
			// to do this with perfect accuracy, and the method
			// used here may result in some false positives,
			// however it wastes at most one byte.
			size_t count = strlen(hostname.get());
			if (count < hostname_len - 2) {
				// If the buffer has been left with 2 or
				// more spare bytes then the hostname
				// was definitely not truncated.
				break;
			}
		}

		// If there is a possibility that the hostname was truncated
		// then double the size of the buffer and try again.
		hostname_len *= 2;
	}

	// Canonicalise the hostname to obtain it in fully-qualified form.
	struct addrinfo hints = {0};
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_CANONNAME;

	struct addrinfo* res = 0;
	if (getaddrinfo(hostname.get(), 0, &hints, &res) == 0) {
		// The hostname was successfully resolved.
		_hostname = std::string(res->ai_canonname);
		freeaddrinfo(res);
	} else {
		// Not resolved, so fall back to hostname returned by OS.
		_hostname = std::string(hostname.get());
	}
}

} /* namespace horace */
