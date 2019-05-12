// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/record.h"

#include "spoolfile_writer.h"

namespace horace {

spoolfile_writer::spoolfile_writer(const std::string& pathname,
	size_t capacity):
	_fd(pathname, O_RDWR|O_CREAT|O_EXCL, 0644),
	_ow(_fd),
	_size(0),
	_capacity(capacity),
	_first(true) {}

void spoolfile_writer::sync() const {
	_fd.fsync();
}

bool spoolfile_writer::write(const record& rec) {
	// Calculate the number of octets required for this record,
	// including the type and length fields.
	size_t content_len = rec.length();
	size_t full_len = octet_writer::base128_length(rec.type()) +
		octet_writer::base128_length(content_len) + content_len;

	// Return false if this record would cause the spoolfile capacity
	// to be exceeded, except that it is always permissible to write
	// at least one event record to each spoolfile.
	if ((_size + full_len > _capacity) && !_first) {
		return false;
	}

	// Write the record, updating the spoolfile size and first event
	// record flag.
	rec.write(_ow);
	_size += full_len;
	_first = false;
	return true;
}

} /* namespace horace */
