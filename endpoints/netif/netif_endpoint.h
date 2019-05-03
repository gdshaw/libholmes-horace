// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NETIF_ENDPOINT
#define LIBHOLMES_HORACE_NETIF_ENDPOINT

#include "horace/endpoint.h"
#include "horace/event_reader_endpoint.h"

#include "interface.h"

namespace horace {

/** An endpoint class to represent a network interface. */
class netif_endpoint:
	public endpoint,
	public event_reader_endpoint {
private:
	/** The network interface referred to by this endpoint. */
	interface _if;
public:
	/** Construct network interface endpoint.
	 * @param name the name of this endpoint
	 */
	netif_endpoint(const std::string& name);

	/** Get the interface.
	 * @return the interface
	 */
	const interface& netif() const {
		return _if;
	}

	virtual std::unique_ptr<event_reader> make_event_reader();
};

} /* namespace horace */

#endif
