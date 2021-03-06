// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_SESSION_WRITER
#define LIBHOLMES_HORACE_FILE_SESSION_WRITER

#include <memory>

#include "horace/lockfile.h"
#include "horace/simple_session_writer.h"

#include "directory_maker.h"

namespace horace {

class spoolfile_writer;
class file_endpoint;

/** A class for writing sessions to a filestore. */
class file_session_writer:
	public simple_session_writer {
private:
	/** The destination endpoint. */
	file_endpoint* _dst_ep;

	/** The filestore subdirectory pathname. */
	std::string _pathname;

	/** A directory_maker instance for creating the subdirectory. */
	directory_maker _dm;

	/** A file descriptor for synchronising the subdirectory. */
	file_descriptor _fd;

	/** A lockfile for controlling write access to the subdirectory. */
	lockfile _lockfile;

	/** The number to use for the next spoolfile when it is created. */
	uint64_t _next_filenum;

	/** The minimum permitted width for a filenum, in digits. */
	unsigned int _minwidth;

	/** A writer for the spoolfile. */
	std::unique_ptr<spoolfile_writer> _sfw;

	/** True if the endpoint is writable, otherwise false.
	 * This contains the cached result of _dst_ep->writable(), which is
	 * called at the start of each spoolfile but not for each record.
	 */
	bool _writable;

	/** Get the pathname to use for the next spoolfile when it is
	 * created.
	 * This function has the side effect of incrementing the filenum
	 * each time it is called.
	 */
	std::string _next_pathname();

	/** Begin new spoolfile.
	 * @param srec the session record to be written
	 */
	void _begin_spoolfile(const record& srec);

	/** Write a record to the current or next spoolfile.
	 * @param rec the record to be written
	 */
	void _write_record(const record& rec);
protected:
	virtual void handle_session_start(const record& srec);
	virtual void handle_session_end(const record& erec);
	virtual void handle_sync(const record& crec);
	virtual void handle_signature(const record& grec);
	virtual void handle_event(const record& rec);
public:
	/** Construct filestore session writer.
	 * @param dst_ep the destination endpoint
	 * @param srcid the required source ID
	 */
	file_session_writer(file_endpoint& dst_ep,
		const std::string& srcid);

	virtual bool writable();
};

} /* namespace horace */

#endif
