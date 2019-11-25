// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "mongodb_error.h"
#include "mongodb_uri.h"
#include "mongodb_collection.h"
#include "mongodb_database.h"

namespace horace {

/** A class for initialising the mongoc library.
 * This class is included within the same compilation unit as
 * mongodb_database to ensure that it is linked into the executable
 * when necessary. It does not need to be publically accessible,
 * so need not appear in the header file.
 */
class mongoc_library {
private:
	/** The single instance of this class. */
	static const mongoc_library instance;

	/** Initialise library. */
	mongoc_library();

	/** Cleanup library. */
	~mongoc_library();
};

mongoc_library::mongoc_library() {
	mongoc_init();
}

mongoc_library::~mongoc_library() {
	mongoc_cleanup();
}

const mongoc_library mongoc_library::instance;

mongodb_database::mongodb_database(const std::string& uri_string) {
	mongodb_uri uri(uri_string);
	_dbname = uri.dbname();
	if (_dbname.empty()) {
		throw endpoint_error("MongoDB database name not specified");
	}
	_client = mongoc_client_new_from_uri(uri);
}

mongodb_database::~mongodb_database() {
	mongoc_client_destroy(_client);
}

mongodb_collection& mongodb_database::collection(const std::string& collname)
	const {

	auto f = _collections.find(collname);
	if (f != _collections.end()) {
		return *f->second.get();
	}

	std::shared_ptr<mongodb_collection> coll =
		std::make_shared<mongodb_collection>(
			*_client, _dbname, collname);
	_collections[collname] = coll;
	return *coll.get();
}

} /* namespace horace */
