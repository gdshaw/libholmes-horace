// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/endpoint_error.h"
#include "horace/record.h"

#include "spoolfile.h"
#include "file_endpoint.h"
#include "file_session_writer.h"

namespace horace {

file_session_writer::file_session_writer(file_endpoint& dst_ep,
	const std::string& source_id):
	_dst_ep(&dst_ep),
	session_writer(source_id),
	_pathname(dst_ep.pathname() + "/" + source_id),
	_fd(_pathname, O_RDONLY),
	_lockfile(_pathname + "/.wrlock"),
	_next_filenum(0) {}

std::string file_session_writer::_next_pathname() {
	// Construct the filename for the new spoolfile, incrementing the
	// current filenum.
	spoolfile sf(_next_filenum++, 8);

	// If this caused the filenum to overflow then roll back the filenum
	// and throw an exception.
	if (_next_filenum == 0) {
		--_next_filenum;
		throw endpoint_error("file number overflow");
	}

	// Construct the full pathname for the new spoolfile.
	return _pathname + "/" + sf.filename();
}

void file_session_writer::handle_event(const record& rec) {
	bool written = false;

	// If there is a spoolfile in progress then attempt to write the
	// record to it.
	if (_sfw) {
		written = _sfw->write(rec);
	}

	// If the record has not yet been written, either because there
	// is no spoolfile in progress or because it is full, open a new
	// spoolfile and write to that instead.
	if (!written) {
		_sfw = std::make_unique<spoolfile_writer>(_next_pathname(),
			_dst_ep->filesize());
		written = _sfw->write(rec);
	}

	// Spoolfiles should allow at least one event record to be written,
	// regardless of size, therefore the above is not expected to fail.
	if (!written) {
		throw endpoint_error("failed to write record to new spoolfile");
	}
}

} /* namespace horace */
