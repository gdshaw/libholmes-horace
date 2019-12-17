// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <climits>

#include <unistd.h>
#include <sys/inotify.h>

#include "horace/libc_error.h"

#include "directory_watcher.h"

namespace horace {

static const size_t buffer_size =
	sizeof(struct inotify_event) + NAME_MAX + 1;

directory_watcher::directory_watcher(const std::string& pathname):
	_fd(inotify_init()),
	_buffer(std::make_unique<char[]>(buffer_size)),
	_count(0),
	_index(0) {

	if (_fd == -1) {
		throw libc_error();
	}
	_fd.interruptible(true);
	inotify_add_watch(_fd, pathname.c_str(), IN_CREATE|IN_MODIFY);
}

std::string directory_watcher::read() {
	while (true) {
		// If the buffer is non-empty then return the next
		// buffered filename.
		if (_index < _count) {
			char* ptr = _buffer.get() + _index;
			struct inotify_event* event =
				reinterpret_cast<struct inotify_event*>(ptr);
			std::string filename(event->name);
			size_t size = sizeof(struct inotify_event) +
				event->len;
			_index += size;
			return filename;
		}

		// If the buffer is empty then fill it (blocking if
		// necessary).
		_count = _fd.read(_buffer.get(), buffer_size);
		_index = 0;
	}
}

} /* namespace horace */
