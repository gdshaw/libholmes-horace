// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SPOOLFILE_WRITER
#define LIBHOLMES_HORACE_SPOOLFILE_WRITER

#include <cstddef>

#include "horace/file_descriptor.h"
#include "horace/file_octet_writer.h"

namespace horace {

/** A class for writing records to a spoolfile. */
class spoolfile_writer {
private:
	/** The pathname of this spoolfile. */
	std::string _pathname;

	/** A file descriptor for writing to the spoolfile. */
	file_descriptor _fd;

	/** An octet writer for writing to the spoolfile. */
	file_octet_writer _ow;

	/** The current size of this spoolfile, in octets. */
	size_t _size;

	/** The capacity of this spoolfile, in octets. */
	size_t _capacity;

	/** True if no event records have been written, otherwise false. */
	bool _first;
public:
	/** Construct spoolfile writer.
	 * @param pathname the required pathname
	 * @param capacity the required capacity, in octets
	 */
	explicit spoolfile_writer(const std::string& pathname,
		size_t capacity);

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

	/** Attempt to write record to spoolfile.
	 * This operation will fail if the spoolfile has insufficient
	 * capacity remaining.
	 * @param rec the record to be written
	 * @return true if successful, otherwise false
	 */
	bool write(const record& rec);
};

} /* namespace horace */

#endif
