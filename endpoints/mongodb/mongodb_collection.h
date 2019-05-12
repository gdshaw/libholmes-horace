// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_COLLECTION
#define LIBHOLMES_HORACE_MONGODB_COLLECTION

#include <utility>
#include <string>

#include <mongoc.h>

namespace horace {

/** A class to represent a MongoDB collection. */
class mongodb_collection {
private:
	/** The underlying mongoc collection object. */
	mongoc_collection_t* _collection;
public:
	/** Construct MongoDB collection.
	 * @param client the mongoc client instance
	 * @param dbname the MongoDB database name
	 * @param clname the MongoDB collection name
	 */
	mongodb_collection(mongoc_client_t& client,
		const std::string& dbname, const std::string& clname);

	/** Destroy MongoDB collection. */
	virtual ~mongodb_collection();

	mongodb_collection(const mongodb_collection&) = delete;
	mongodb_collection& operator=(const mongodb_collection&) = delete;

	/** Move-construct MongoDB collection.
	 * @param that the collection to be moved
	 */
	mongodb_collection(mongodb_collection&& that) {
		this->_collection = that._collection;
		that._collection = 0;
	}

	/** Move-assign MongoDB collection.
	 * @param that the collection to be moved
	 * @return a reference to this
	 */
	mongodb_collection& operator=(mongodb_collection&& that) {
		std::swap(this->_collection, that._collection);
		return *this;
	}

	/** Get underlying mongoc collection object.
	 * @return the mongoc object
	 */
	operator mongoc_collection_t*() const {
		return _collection;
	}
};

} /* namespace horace */

#endif
