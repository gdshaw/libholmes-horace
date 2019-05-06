// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/record.h"

#include "spoolfile_writer.h"

namespace horace {

spoolfile_writer::spoolfile_writer(const std::string& pathname):
	_fd(pathname, O_RDWR|O_CREAT|O_EXCL, 0644),
	_ow(_fd) {}

void spoolfile_writer::sync() const {
	_fd.fsync();
}

void spoolfile_writer::write(const record& rec) {
	rec.write(_ow);
}

} /* namespace horace */
