// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fstream>

#include "horace/keypair.h"
#include "horace/ed25519_keypair.h"

namespace horace {

std::unique_ptr<keypair> keypair::load(const std::string& pathname) {
	std::ifstream infile(pathname);
	std::string line;
	std::getline(infile, line);
	std::string::size_type f = line.find(':');
	if (f == std::string::npos) {
		throw std::runtime_error("malformed keyfile");
	}
	std::string alg_name = line.substr(0, f);

	if (alg_name == "ed25519") {
		return std::make_unique<ed25519_keypair>(line);
	} else {
		throw std::runtime_error("unrecognised algorithm in keyfile");
	}
};

} /* namespace horace */
