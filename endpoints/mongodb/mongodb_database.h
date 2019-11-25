// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_DATABASE
#define LIBHOLMES_HORACE_MONGODB_DATABASE

#include <memory>
#include <map>

#include <mongoc.h>

namespace horace {

class mongodb_collection;

/** An class to represent a MongoDB database.
 * Separate instances of this class may be used by different threads,
 * however individual instances may not.
 */
class mongodb_database {
private:
	/** The underlying mongoc client. */
	mongoc_client_t* _client;

	/** The MongoDB database name. */
	std::string _dbname;

	/** Cached MongoDB collection objects, indexed by name. */
	mutable std::map<std::string, std::shared_ptr<mongodb_collection>>
		_collections;
public:
	/** Open connection to database.
	 * @param name the MongoDB connection URI
	 */
	mongodb_database(const std::string& uri_string);

	/** Close connection to database. */
	virtual ~mongodb_database();

	mongodb_database(const mongodb_database&) = delete;
	mongodb_database& operator=(const mongodb_database&) = delete;

	/** Access collection.
	 * @param clname the collection name
	 * @return a reference to the collection
	 */
	mongodb_collection& collection(const std::string& clname) const;
};

} /* namespace horace */

#endif
