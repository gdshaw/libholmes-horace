// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/hash.h"

namespace horace {

hash::hash():
	buffer(0x40),
	octet_writer(buffer_ptr(), buffer_size()) {}

} /* namespace horace */
