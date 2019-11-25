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
	size_t hostname_len = 64;
	std::unique_ptr<char[]> hostname;
	while (1) {
		hostname = std::make_unique<char[]>(hostname_len);
		hostname[hostname_len - 1] = 0;
		if (gethostname(hostname.get(), hostname_len - 1) == 0) {
			size_t count = strlen(hostname.get());
			if (count < hostname_len - 2) {
				// Hostname definitely not truncated.
				break;
			}
		}

		// Hostname might have been truncated.
		// Double size of buffer and try again.
		hostname_len *= 2;
	}

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
