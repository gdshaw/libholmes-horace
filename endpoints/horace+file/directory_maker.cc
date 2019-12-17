// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cerrno>

#include <sys/stat.h>

#include "horace/libc_error.h"

#include "directory_maker.h"

namespace horace {

directory_maker::directory_maker(const std::string& pathname) {
	if (mkdir(pathname.c_str(), 0777) == -1) {
		if (errno != EEXIST) {
			throw libc_error();
		}
	}
}

} /* namespace horace */
