// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SPOOLFILE
#define LIBHOLMES_HORACE_SPOOLFILE

#include <string>

namespace horace {

/** A class to represent a spoolfile. */
class spoolfile {
private:
	/** The filename of the spoolfile. */
	std::string _filename;

	/** The filenum of the spoolfile. */
	unsigned long _filenum;
public:
	/** Construct spoolfile from filename.
	 * @param filename the filename
	 */
	explicit spoolfile(const std::string& filename);

	/** Construct spoolfile from filenum.
	 * @param filenum the filenum
	 * @param minwidth the minimum width
	 */
	spoolfile(unsigned long filenum, unsigned int minwidth);

	/** Get the filename of this spoolfile.
	 * @return the filename
	 */
	const std::string& filename() const {
		return _filename;
	}

	/** Get the filenum of this spoolfile.
	 * @return the filenum
	 */
	unsigned long filenum() const {
		return _filenum;
	}

	/** Test whether the filename is padded.
	 * @return true if padded, otherwise false
	 */
	bool has_padding() const {
		return _filename.length() && (_filename[0] == '0');
	}
};

} /* namespace horace */

#endif
