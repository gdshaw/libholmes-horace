// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_LOCKFILE
#define LIBHOLMES_HORACE_LOCKFILE

#include <string>

#include "horace/file_descriptor.h"

namespace horace {

/** A class to represent a lockfile within a filestore.
 * This class attempts to acquire an advisory lock on the file at the given
 * pathname, creating it if necessary. If the lock cannot be acquired
 * immediately then an exception is thrown. The lock is an exclusive one and
 * is created using flock.
 *
 * While an instance of this class exists, no other process will be able to
 * acquire a lock for the same pathname using the same method. If the
 * instance is destroyed then the lock will be released and the lockfile
 * deleted (whether or not that instance was responsible for creating it).
 * Alternatively, if the process which created the instance terminates
 * without first destroying it then the lock will be released. The lockfile
 * will not necessarily be deleted under these circumstances, however that
 * does not prevent a future process from acquiring the same lock.
 *
 * Lockfiles are movable but not copyable.
 */
class lockfile {
private:
	/** The pathname of the lockfile. */
	std::string _pathname;

	/** The file descriptor for the lockfile. */
	file_descriptor _fd;
public:
	/** Create lockfile.
	 * @param pathname the lockfile pathname
	 */
	lockfile(const std::string& pathname);

	/** Remove lockfile. */
	~lockfile();
};

} /* namespace horace */

#endif
