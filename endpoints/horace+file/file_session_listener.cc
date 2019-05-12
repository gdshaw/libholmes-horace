// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "directory.h"
#include "file_endpoint.h"
#include "file_session_reader.h"
#include "file_session_listener.h"

namespace horace {

void file_session_listener::_scan() {
	directory dir(_pathname);
	while (dir) {
		std::string filename = dir.read();
		if (!filename.empty() && (filename[0] != '.')) {
			if ((_pending.find(filename) == _pending.end()) &&
				(_accepted.find(filename) == _accepted.end())) {

				_pending.insert(filename);
			}
		}
	}
}

file_session_listener::file_session_listener(file_endpoint& src_ep):
	_src_ep(&src_ep),
	_pathname(src_ep.pathname()),
	_watcher(_pathname) {

	_scan();
}

std::unique_ptr<session_reader> file_session_listener::accept() {
	while (true) {
		if (!_pending.empty()) {
			std::string source_id = *_pending.begin();
			_accepted.insert(source_id);
			_pending.erase(source_id);

			std::string pathname = _pathname + "/" + source_id;
			return std::make_unique<file_session_reader>(*_src_ep, source_id);
		}

		_watcher.read();
		_scan();
	}
}

} /* namespace horace */
