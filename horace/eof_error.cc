// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <horace/eof_error.h>

namespace horace {

eof_error::eof_error():
	std::runtime_error("unexpected end of file") {}

} /* namespace horace */
