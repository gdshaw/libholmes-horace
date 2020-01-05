// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/source_id.h"

namespace horace {

source_id::source_id(const std::string& id):
	_id(id) {

	if (_id.empty()) {
		throw horace_error("invalid protocol ID (empty string)");
	}
	if (_id.length() > 255) {
		throw horace_error("invalid protocol ID (too long)");
	}
	for (char c : _id) {
		if (!isalnum(c) && (c != '-') && (c != '.')) {
			throw horace_error("invalid protocol ID (invalid character)");
		}
	}
	if (_id[0] == '.') {
		throw horace_error("invalid protocol ID (initial full stop)");
	}
}

} /* namespace horace */
