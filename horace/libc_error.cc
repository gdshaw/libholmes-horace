// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cerrno>
#include <cstring>

#include "horace/libc_error.h"

namespace horace {

const char* libc_error_holder::strerror() const {
	// Using strerror_l because strerror is not thread safe,
	// and strerror_r defaults to a non-standard implementation
	// in glibc.
	static const locale_t locale = newlocale(0, "POSIX", 0);
	return strerror_l(_errno, locale);
}

libc_error::libc_error():
	libc_error_holder(errno),
	std::runtime_error(strerror()) {}

libc_error::libc_error(int errno_arg):
	libc_error_holder(errno_arg),
	std::runtime_error(strerror()) {}

} /* namespace horace */
