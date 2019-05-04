// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/endpoint_error.h"

#include "null_endpoint.h"

namespace horace {

null_endpoint::null_endpoint(const std::string& name):
	endpoint(name) {

	std::string path = this->name().path();
	if (!path.empty()) {
		throw endpoint_error(
			"null endpoint name must have empty path");
	}
}

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::null_endpoint>(name);
}
