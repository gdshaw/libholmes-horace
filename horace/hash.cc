// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/hash.h"
#include "horace/sha256.h"

namespace horace {

hash::hash():
	buffer(0x40),
	octet_writer(buffer_ptr(), buffer_size()) {}

std::unique_ptr<hash> hash::make(const std::string& name) {
	if (name == "sha256") {
		return std::make_unique<sha256>();
	} else {
		throw std::invalid_argument("unrecognised hash function name");
	}
}

} /* namespace horace */
