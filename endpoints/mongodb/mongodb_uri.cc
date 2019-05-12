// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/endpoint_error.h"

#include "mongodb_error.h"
#include "mongodb_uri.h"

namespace horace {

mongodb_uri::mongodb_uri(const std::string& uri_string) {
	bson_error_t error;
	_uri = mongoc_uri_new_with_error(uri_string.c_str(), &error);
	if (!_uri) {
		throw mongodb_error(error);
	}
}

mongodb_uri::~mongodb_uri() {
	mongoc_uri_destroy(_uri);
}

std::string mongodb_uri::dbname() const {
	const char* _dbname = mongoc_uri_get_database(_uri);
	return std::string(_dbname ? _dbname : "");
}

} /* namespace horace */
