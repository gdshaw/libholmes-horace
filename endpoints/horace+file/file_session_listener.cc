// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/session_context.h"

#include "directory.h"
#include "spoolfile_reader.h"
#include "file_endpoint.h"
#include "file_session_reader.h"
#include "file_session_listener.h"

namespace horace {

void file_session_listener::_scan() {
	// Iterate over the names in the directory.
	directory dir(_pathname);
	while (dir) {
		std::string filename = dir.read();

		// Exclude names beginning with a dot to ensure that
		// session readers are not created for the current
		// directory, the parent directory, or any lockfiles.
		if (!filename.empty() && (filename[0] != '.')) {
			// Exclude sources in the accepted set, to avoid
			// creating more than one session reader for a
			// given source.
			// (No need to check the pending set because it
			// shouldn't be in there, and even if it was
			// inserting a second time would have no effect.)
			if (_accepted.find(filename) == _accepted.end()) {
				_pending.insert(filename);
			}
		}
	}
}

file_session_listener::file_session_listener(file_endpoint& src_ep):
	_src_ep(&src_ep),
	_pathname(src_ep.pathname()),
	_watcher(_pathname) {

	// Existing sources will not be detected by inotify, therefore
	// an initial scan is needed.
	_scan();
}

std::unique_ptr<session_reader> file_session_listener::accept() {
	while (true) {
		// Create file session readers for any sources in the
		// pending set, moving them to the accepted set once
		// handled.
		if (!_pending.empty()) {
			std::string srcid = *_pending.begin();
			_accepted.insert(srcid);
			_pending.erase(srcid);
			return std::make_unique<file_session_reader>(
				*_src_ep, srcid);
		}

		// Wait for any addition to the directory.
		// Occasional false positives are tolerable here, since the
		// objective is merely to avoid having to scan the
		//  directory continuously.
		_watcher.read();

		// Rescan the directory to pick up any new sources.
		// This method is somewhat inefficient, but new sources are
		// expected to be sufficiently infrequent for that not to
		// be a concern. It does have the virtue of not needing to
		// detect and handle overruns in the inotify queue, which
		// might otherwise cause sources to be missed.
		_scan();
	}
}

} /* namespace horace */
