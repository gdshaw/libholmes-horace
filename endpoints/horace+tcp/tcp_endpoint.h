// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TCP_ENDPOINT
#define LIBHOLMES_HORACE_TCP_ENDPOINT

#include <string>

#include "horace/endpoint.h"

namespace horace {

/** An endpoint class for making and receiving connections via TCP. */
class tcp_endpoint:
	public endpoint {
private:
	/** The hostname. */
	std::string _hostname;

	/** The portname. */
	std::string _portname;
public:
	/** Construct TCP endpoint.
	 * @param name the name of this endpoint
	 */
	explicit tcp_endpoint(const std::string& name);

	/** Get the hostname.
	 * @return the hostname
	 */
	const std::string& hostname() const {
		return _hostname;
	}

	/** Get the portname.
	 * @return the portname
	 */
	const std::string& portname() const {
		return _portname;
	}
};

} /* namespace horace */

#endif
