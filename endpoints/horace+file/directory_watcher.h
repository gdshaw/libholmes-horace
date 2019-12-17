// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_DIRECTORY_WATCHER
#define LIBHOLMES_HORACE_DIRECTORY_WATCHER

#include <memory>
#include <string>

#include "horace/file_descriptor.h"

namespace horace {

/** A class for monitoring a directory for newly-created entries. */
class directory_watcher {
private:
	/** The file descriptor used to watch the directory. */
	file_descriptor _fd;

	/** A buffer for receiving events. */
	std::unique_ptr<char[]> _buffer;

	/** The number of occupied octets in the buffer. */
	size_t _count;

	/** The index of the first unused octet in the buffer,
	 * or _count if there are no unused octets. */
	size_t _index;
public:
	/** Watch directory.
	 * @param pathname the directory pathname
	 */
	directory_watcher(const std::string& pathname);

	/** Read the next file/directory name.
	 * If there are no names waiting to be read then this function
	 * will block until one is available.
	 * @return the name of a newly-created file or directory
	 */
	std::string read();
};

} /* namespace horace */

#endif
