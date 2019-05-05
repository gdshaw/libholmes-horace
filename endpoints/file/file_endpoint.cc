// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "file_endpoint.h"

namespace horace {

file_endpoint::file_endpoint(const std::string& name):
	endpoint(name),
	_pathname(this->name().path()) {}

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::file_endpoint>(name);
}
