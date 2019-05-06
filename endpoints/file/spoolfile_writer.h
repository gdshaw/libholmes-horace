// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SPOOLFILE_WRITER
#define LIBHOLMES_HORACE_SPOOLFILE_WRITER

#include "horace/file_descriptor.h"
#include "horace/file_octet_writer.h"

namespace horace {

/** A class for writing records to a spoolfile. */
class spoolfile_writer {
private:
	/** A file descriptor for writing to the spoolfile. */
	file_descriptor _fd;

	/** An octet writer for writing to the spoolfile. */
	file_octet_writer _ow;
public:
	/** Construct spoolfile writer.
	 * @param pathname the required pathname
	 */
	explicit spoolfile_writer(const std::string& pathname);

	spoolfile_writer(const spoolfile_writer&) = delete;
	spoolfile_writer& operator=(const spoolfile_writer&) = delete;
	spoolfile_writer(spoolfile_writer&& that) = delete;
	spoolfile_writer& operator=(spoolfile_writer&& that) = delete;

	/** Ensure that the spoolfile content has been written durably.
	 * Note that this function does not ensure the durability of the
	 * spoolfile directory entry, nor of any entries further up the
	 * directory hierarchy.
	 */
	void sync() const;

	/** Write record to spoolfile.
	 * @param rec the record to be written
	 */
	void write(const record& rec);
};

} /* namespace horace */

#endif
