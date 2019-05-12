// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "mongodb_collection.h"

namespace horace {

mongodb_collection::mongodb_collection(mongoc_client_t& client,
	const std::string& dbname, const std::string& clname):
	_collection(mongoc_client_get_collection(
		&client, dbname.c_str(), clname.c_str())) {}

mongodb_collection::~mongodb_collection() {
	if (_collection) {
		mongoc_collection_destroy(_collection);
	}
}

} /* namespace horace */
