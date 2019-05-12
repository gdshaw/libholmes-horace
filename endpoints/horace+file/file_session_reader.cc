// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/eof_error.h"
#include "horace/endpoint_error.h"

#include "filestore_scanner.h"
#include "spoolfile.h"
#include "file_endpoint.h"
#include "file_session_reader.h"

namespace horace {

file_session_reader::file_session_reader(file_endpoint& src_ep,
	const std::string& source_id):
	_src_ep(&src_ep),
	_pathname(src_ep.pathname() + "/" + source_id),
	_dm(_pathname),
	_fd(_pathname, O_RDONLY),
	_watcher(_pathname),
	_lockfile(_pathname + "/.rdlock"),
	_next_filenum(0),
	_minwidth(0) {}

std::string file_session_reader::_next_pathname() {
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

std::unique_ptr<record> file_session_reader::read() {
	while (true) {
		// If there is an open spoolfile then attempt to read a record from it.
		if (_sfr) {
			try {
				return _sfr->read();
			} catch (eof_error& ex) {
				// No action required.
			}
		}

		// If the above fails then attempt to open a new spoolfile.
		//
		// First scan to find first filenum. If the filestore is
		// empty then wait until at least one spoolfile is available.
		while (_minwidth == 0) {
			filestore_scanner scanner(_pathname);
			if (scanner.minwidth() != 0) {
				_next_filenum = scanner.first_filenum();
				_minwidth = scanner.minwidth();
			} else {
				wait();
			}
		}

		// Now open the spoolfile.
		std::string next_pathname = (_sfr) ?
			_sfr->next_pathname() :
			_next_pathname();
		_sfr = std::make_unique<spoolfile_reader>(
			*this, next_pathname, _next_pathname());
	}
}

void file_session_reader::wait() {
	_watcher.read();
}

} /* namespace horace */
