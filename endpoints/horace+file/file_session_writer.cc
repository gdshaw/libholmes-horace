// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/endpoint_error.h"
#include "horace/record.h"

#include "spoolfile.h"
#include "spoolfile_writer.h"
#include "filestore_scanner.h"
#include "file_endpoint.h"
#include "file_session_writer.h"

namespace horace {

std::string file_session_writer::_next_pathname() {
	// Construct the filename for the new spoolfile, incrementing the
	// current filenum.
	spoolfile sf(_next_filenum++, _minwidth);

	// If this caused the filenum to overflow then roll back the filenum
	// and throw an exception.
	if (_next_filenum == 0) {
		--_next_filenum;
		throw endpoint_error("file number overflow");
	}

	// Construct the full pathname for the new spoolfile.
	return _pathname + "/" + sf.filename();
}

void file_session_writer::_begin_spoolfile(const record& srec) {
	if (_sfw) {
		_sfw->sync();
	}
	_sfw = std::make_unique<spoolfile_writer>(_next_pathname(),
		_dst_ep->filesize());
	bool written = _sfw->write(srec);
	if (!written) {
		throw endpoint_error(
			"failed to write session record to new spoolfile");
	}
	_fd.fsync();

	_writable = _dst_ep->writable();
}

void file_session_writer::_write_record(const record& rec) {
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
		_begin_spoolfile(session_record());
		written = _sfw->write(rec);
	}

	// Spoolfiles should allow at least one non-session record to be
	// written, regardless of size, therefore the above is not expected
	// to fail.
	if (!written) {
		throw endpoint_error("failed to write record to new spoolfile");
	}
}

file_session_writer::file_session_writer(file_endpoint& dst_ep,
	const std::string& srcid):
	simple_session_writer(srcid),
	_dst_ep(&dst_ep),
	_pathname(dst_ep.pathname() + "/" + srcid),
	_dm(_pathname),
	_fd(_pathname, O_RDONLY),
	_lockfile(_pathname + "/.wrlock"),
	_writable(false) {

	filestore_scanner scanner(_pathname);
	_next_filenum = scanner.next_filenum();
	_minwidth = scanner.minwidth();
	if (_minwidth == 0) {
		_minwidth = 6;
	}
}

void file_session_writer::handle_session_start(const record& srec) {
	_begin_spoolfile(srec);
}

void file_session_writer::handle_session_end(const record& srec) {
	_sfw->sync();
	_sfw = 0;
}

void file_session_writer::handle_session_update(const record& srec) {
	_write_record(srec);
}

void file_session_writer::handle_sync(const record& crec) {
	_sfw->sync();
}

void file_session_writer::handle_signature(const record& grec) {
	_write_record(grec);
}

void file_session_writer::handle_event(const record& rec) {
	_write_record(rec);
}

bool file_session_writer::writable() {
	if (!_writable) {
		_writable = _dst_ep->writable();
	}
	return _writable;
}

} /* namespace horace */
