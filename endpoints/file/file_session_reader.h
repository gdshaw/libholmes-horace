// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_SESSION_READER
#define LIBHOLMES_HORACE_FILE_SESSION_READER

#include "horace/record.h"
#include "horace/session_reader.h"

#include "directory_maker.h"
#include "directory_watcher.h"
#include "lockfile.h"
#include "spoolfile_reader.h"

namespace horace {

class file_endpoint;

/** A class for reading sessions from a file endpoint. */
class file_session_reader:
	public session_reader {
public:
	/** The source endpoint. */
	file_endpoint* _src_ep;

	/** The filestore subdirectory pathname. */
	std::string _pathname;

	/** A file descriptor for synchronising the subdirectory. */
	file_descriptor _fd;

	/** A directory_maker instance for creating the subdirectory. */
	directory_maker _dm;

	/** A directory watcher for monitoring changes to this filestore. */
	directory_watcher _watcher;

	/** A lockfile for controlling read access to the subdirectory. */
	lockfile _lockfile;

	/** The number to use for the next spoolfile when it is created. */
	uint64_t _next_filenum;

	/** The minimum permitted width for a filenum, in digits. */
	unsigned int _minwidth;

	/** A reader for the current spoolfile. */
	std::unique_ptr<spoolfile_reader> _sfr;

	/** Get the pathname at which to look for the next spoolfile.
	 * This function has the side effect of incrementing the filenum
	 * each time it is called.
	 */
	std::string _next_pathname();
public:
	/** Construct filestore session reader.
	 * @param src_ep the source endpoint
	 * @param source_id the source ID
	 */
	file_session_reader(file_endpoint& src_ep,
		const std::string& source_id);

	virtual std::unique_ptr<record> read();

	/** Wait for a change to the repository.
	 * This could be a change to an existing file or the creation
	 * of a new file.
	 */
	void wait();
};

} /* namespace horace */

#endif
