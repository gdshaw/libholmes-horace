// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cerrno>
#include <cstring>

#include "horace/libc_error.h"

namespace horace {

libc_error::libc_error():
	std::runtime_error(strerror(errno)) {}

libc_error::libc_error(int errno_arg):
	std::runtime_error(strerror(errno_arg)) {}

} /* namespace horace */
