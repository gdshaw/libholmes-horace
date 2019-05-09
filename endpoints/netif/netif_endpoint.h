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

	/** The capture method. */
	std::string _method;

	/** The snaplen, in octets. */
	long _snaplen;

	/** The buffer capacity, in octets. */
	size_t _capacity;

	/** True if promiscuous mode requested, otherwise false. */
	bool _promiscuous;
public:
	/** Construct network interface endpoint.
	 * @param name the name of this endpoint
	 */
	explicit netif_endpoint(const std::string& name);

	/** Get the interface.
	 * @return the interface
	 */
	const interface& netif() const {
		return _if;
	}

	/** Get the capture method.
	 * @return the capture method
	 */
	const std::string& method() const {
		return _method;
	}

	/** Get the snaplen.
	 * @return the snaplen, in octets
	 */
	long snaplen() const {
		return _snaplen;
	}

	/** Get the buffer capacity.
	 * @return the capacity, in octets
	 */
	size_t capacity() const {
		return _capacity;
	}

	/** Check whether promiscuous mode requested.
	 * @return true if promiscuous mode requested, otherwise false
	 */
	bool promiscuous() const {
		return _promiscuous;
	}

	virtual std::unique_ptr<event_reader> make_event_reader();
};

} /* namespace horace */

#endif
