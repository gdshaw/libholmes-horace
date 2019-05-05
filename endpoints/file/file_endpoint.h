// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_ENDPOINT
#define LIBHOLMES_HORACE_FILE_ENDPOINT

#include <string>

#include "horace/endpoint.h"

namespace horace {

/** An endpoint class to represent a set of HORACE filestores. */
class file_endpoint:
	public endpoint {
private:
	/** The pathname for this endpoint. */
	std::string _pathname;
public:
	/** Construct file endpoint.
	 * @param name the name of this endpoint
	 */
	file_endpoint(const std::string& name);

	/** Get the pathname.
	 * @return the pathname
	 */
	const std::string& pathname() const {
		return _pathname;
	}
};

} /* namespace horace */

#endif
