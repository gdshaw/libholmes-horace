// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/terminate_exception.h"

namespace horace {

const char* terminate_exception::what() const noexcept {
	return "terminate";
}

} /* namespace horace */
