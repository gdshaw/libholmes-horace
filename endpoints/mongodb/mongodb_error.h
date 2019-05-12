// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_ERROR
#define LIBHOLMES_HORACE_MONGODB_ERROR

#include <mongoc.h>

#include "horace/endpoint_error.h"

namespace horace {

/** A class to represent an error originating from MongoDB. */
class mongodb_error:
	public endpoint_error {
public:
	/** Construct MongoDB error.
	 * @param error a bson_error_t containing the error message
	 */
	mongodb_error(const bson_error_t& error):
		endpoint_error(error.message) {}
};

} /* namespace horace */

#endif
