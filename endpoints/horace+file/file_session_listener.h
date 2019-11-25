// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_FILE_SESSION_LISTENER
#define LIBHOLMES_HORACE_FILE_SESSION_LISTENER

#include <set>

#include "horace/session_listener.h"

#include "directory_watcher.h"

namespace horace {

class file_endpoint;

/** A class for listening for sessions from a file endpoint. */
class file_session_listener:
	public session_listener {
public:
	/** The source endpoint. */
	file_endpoint* _src_ep;

	/** The pathname of the filestore root directory. */
	std::string _pathname;

	/** A directory watcher for monitoring changes to this filestore. */
	directory_watcher _watcher;

	/** The set of sources already accepted. */
	std::set<std::string> _accepted;

	/** The set of sources observed but not yet accepted. */
	std::set<std::string> _pending;

	/** Scan the directory for new sources. */
	void _scan();
public:
	/** Construct filestore session listener.
	 * @param src_ep the source endpoint
	 */
	file_session_listener(file_endpoint& src_ep);

	virtual std::unique_ptr<session_reader> accept();
};

} /* namespace horace */

#endif
