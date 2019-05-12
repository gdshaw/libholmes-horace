// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>
#include <climits>

#include "directory.h"
#include "spoolfile.h"
#include "filestore_scanner.h"

namespace horace {

filestore_scanner::filestore_scanner(const std::string& pathname):
	_first_filenum(ULONG_MAX),
	_next_filenum(0),
	_minwidth(UINT_MAX) {

	bool minwidth_fixed = false;
	directory dir(pathname);
	while (dir) {
		std::string filename = dir.read();
		if (filename.length() && (filename[0] != '.')) {
			spoolfile sf(filename);
			unsigned long filenum = sf.filenum();
			if (filenum == ULONG_MAX) {
				throw std::runtime_error("file number overflow");
			}

			if (!minwidth_fixed) {
				if (sf.has_padding()) {
					// Had padding, minwidth not yet fixed:
					// Fix minwidth, after checking that it is
					// consistent with previous filenames.
					if (filename.length() > _minwidth) {
						throw std::runtime_error("inconsistent padding in filestore");
					}
					_minwidth = filename.length();
					minwidth_fixed = true;
				} else {
					// No padding, minwidth not yet fixed:
					// Cannot be inconsistent with previous
					// filenames, but may constrain future ones.
					if (filename.length() < _minwidth) {
						_minwidth = filename.length();
					}
				}
			} else {
				if (sf.has_padding()) {
					// Has padding, minwidth is fixed:
					// Must exactly match minwidth.
					if (filename.length() != _minwidth) {
						throw std::runtime_error("inconsistent padding in filestore");
					}
				} else {
					// No padding, minwidth is fixed:
					// Must be consistent with minwidth.
					if (filename.length() < _minwidth) {
						throw std::runtime_error("inconsistent padding in filestore");
					}
				}
			}

			if (filenum < _first_filenum) {
				_first_filenum = filenum;
			}
			if (filenum >= _next_filenum) {
				_next_filenum = filenum + 1;
			}
		}
	}

	if (_first_filenum == ULONG_MAX) {
		_first_filenum = 0;
	}
	if (_minwidth == UINT_MAX) {
		_minwidth = 0;
	}
}

} /* namespace horace */
