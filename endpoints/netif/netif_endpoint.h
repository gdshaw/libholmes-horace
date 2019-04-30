// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NETIF_ENDPOINT
#define LIBHOLMES_HORACE_NETIF_ENDPOINT

#include "horace/endpoint.h"

namespace horace {

/** An endpoint class to represent a network interface. */
class netif_endpoint:
	public endpoint {
public:
	/** Construct network interface endpoint.
	 * @param name the name of this endpoint
	 */
	netif_endpoint(const std::string& name);
};

} /* namespace horace */

#endif
