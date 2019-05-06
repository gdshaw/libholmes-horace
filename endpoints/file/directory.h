// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_DIRECTORY
#define LIBHOLMES_HORACE_DIRECTORY

#include <string>

#include <dirent.h>

namespace horace {

/** A class for reading the filenames in a directory. */
class directory {
private:
	/** A pointer to the open directory stream. */
	DIR* _dp;

	/** A pointer to the current directory entry, or null if none. */
	struct dirent* _ep;
public:
	/** Open directory.
	 * @param pathname the directory pathname
	 */
	directory(const std::string& pathname);

	/** Close directory. */
	~directory();

	/** Check whether another filename can be read. */
	operator bool();

	/** Read the next filename.
	 * @return the next filename
	 */
	std::string read();
};

} /* namespace horace */

#endif
