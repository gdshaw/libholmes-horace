// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_DIRECTORY_MAKER
#define LIBHOLMES_HORACE_DIRECTORY_MAKER

#include <string>

namespace horace {

/** A class for making new directories.
 * This class is intended for use where a directory must be created by
 * a constructor prior to one or more of the class members being
 * initialised.
 *
 * It is not an error if the directory already exists.
 *
 * Note that the case where a different type of object exists with the
 * specified pathname is not currently detected by this class (but will
 * typically result in a ENOENT error when an attempt is made to use
 * the directory).
 */
class directory_maker {
public:
	/** Make directory
	 * @param pathname the directory pathname
	 */
	directory_maker(const std::string& pathname);
};

} /* namespace horace */

#endif
