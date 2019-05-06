// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ENDPOINT
#define LIBHOLMES_HORACE_ENDPOINT

#include <memory>
#include <string>

#include "horace/uri.h"

namespace horace {

/** A base class to represent a data transfer endpoint.
 * By itself this class does not give the endpoint any substantive
 * capabilities. It is expected that endpoint implementations will
 * normally inherit from one or more mix-in classs corresponding to
 * the capabilities which are applicable. A dynamic cast should be
 * used to test for and gain access to a given capability.
 */
class endpoint {
private:
	/** The name of this endpoint, as a parsed URI. */
	uri _name;
public:
	/** Construct endpoint.
	 * @param name the name of this endpoint
	 */
	explicit endpoint(const std::string& name);

	/** Destroy endpoint. */
	virtual ~endpoint() = default;

	/** Get the name of this endpoint.
	 * @return the name, as a parsed URI.
	 */
	const uri& name() const {
		return _name;
	}

	/** Make endpoint from name.
	 * @param name the required endpoint name
	 * @return the resulting endpoint, or 0 if none
	 */
	static std::unique_ptr<endpoint> make(const std::string& name);
};

} /* namespace horace */

#endif
