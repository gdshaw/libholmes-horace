// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_URI
#define LIBHOLMES_HORACE_MONGODB_URI

#include <mongoc.h>

namespace horace {

/** A class to represent a MongoDB URI. */
class mongodb_uri {
private:
	/** The MongoDB uri. */
	mongoc_uri_t* _uri;
public:
	/** Construct MongoDB URI from string. */
	mongodb_uri(const std::string& uri_string);

	/** Destroy MongoDB URI. */
	virtual ~mongodb_uri();

	mongodb_uri(const mongodb_uri&) = delete;
	mongodb_uri& operator=(const mongodb_uri&) = delete;

	operator mongoc_uri_t*() const {
		return _uri;
	}

	/** Get database name.
	 * @return the database name, or the empty string if none
	 */
	std::string dbname() const;
};

} /* namespace horace */

#endif
