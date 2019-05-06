// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILESTORE_SCANNER
#define LIBHOLMES_HORACE_FILESTORE_SCANNER

#include <string>

namespace horace {

/** A class for scanning the existing content of a filestore. */
class filestore_scanner {
private:
	/** The first file number in the filestore, or 0 if the filestore
	 * is empty. */
	unsigned long _first_filenum;

	/** The next file number following the last in the filestore,
	 * or 0 if the filestore is empty. */
	unsigned long _next_filenum;

	/** The minimum width of a filenum, in digits, or 0 if the filestore
	 * is empty. */
	unsigned int _minwidth;
public:
	/** Scan filestore.
	 * @param pathname the filestore pathname
	 */
	filestore_scanner(const std::string& pathname);

	/** Get the first file number in the filestore.
	 * @return the first file number, or 0 if filestore empty
	 */
	unsigned long first_filenum() const {
		return _first_filenum;
	}

	/** Get the next file number following the last in the filestore.
	 * @return the next file number, or 0 if filestore empty
	 */
	unsigned long next_filenum() const {
		return _next_filenum;
	}

	/** Get the minimum width of file numbers in the filestore.
	 * @return the minimum width, in digits, or 0 if filestore empty
	 */
	unsigned int minwidth() const {
		return _minwidth;
	}
};

} /* namespace horace */

#endif
