// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_SESSION_READER
#define LIBHOLMES_HORACE_FILE_SESSION_READER

#include "horace/lockfile.h"
#include "horace/record.h"
#include "horace/session_context.h"
#include "horace/session_reader.h"

#include "directory_maker.h"
#include "directory_watcher.h"

namespace horace {

class record;

class spoolfile_reader;
class file_endpoint;

/** A class for reading sessions from a file endpoint. */
class file_session_reader:
	public session_reader {
public:
	/** The source endpoint. */
	file_endpoint* _src_ep;

	/** The source identifier. */
	std::string _srcid;

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

	/** The current session context. */
	session_context _session;

	/** A reader for the current spoolfile. */
	std::unique_ptr<spoolfile_reader> _sfr;

	/** The current session timestamp. */
	struct timespec _session_ts;

	/** The current sequence number. */
	uint64_t _seqnum;

	/** True if waiting for a sync record, otherwise false. */
	bool _awaiting_sync;

	/** Get the pathname at which to look for the next spoolfile.
	 * This function has the side effect of incrementing the filenum
	 * each time it is called.
	 */
	std::string _next_pathname();

	/** Handle a sync record.
	 * @param rec the sync record
	 */
	void _handle_sync(const record& rec);
public:
	/** Construct filestore session reader.
	 * @param src_ep the source endpoint
	 * @param srcid the source ID
	 */
	file_session_reader(file_endpoint& src_ep,
		const std::string& srcid);

	virtual std::unique_ptr<record> read();
	virtual void write(const record& rec);
	virtual bool reset();

	/** Wait for a change to the repository.
	 * This could be a change to an existing file or the creation
	 * of a new file.
	 */
	void wait();
};

} /* namespace horace */

#endif
