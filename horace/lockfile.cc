// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>

#include "horace/endpoint_error.h"
#include "horace/lockfile.h"

namespace horace {

lockfile::lockfile(const std::string& pathname):
	_pathname(pathname),
	_fd(_pathname, O_WRONLY|O_CREAT, 0600) {

	if (!_fd.lock()) {
		throw endpoint_error("filestore is locked for writing");
	}
}

lockfile::~lockfile() {
	_fd.unlock();
	unlink(_pathname.c_str());
}

} /* namespace horace */
