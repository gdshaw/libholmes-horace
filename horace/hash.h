// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_HASH
#define LIBHOLMES_HORACE_HASH

#include "horace/buffer.h"
#include "horace/octet_writer.h"

namespace horace {

/** An octet writer class for calculating a secure hash. */
class hash:
	public buffer,
	public octet_writer {
public:
	/** Construct hashing octet writer. */
	hash();

	/** Get the hash of the octet sequence.
	 * @return the hash
	 */
	virtual const void* final() = 0;

	/** Get the length of the hash
	 * @return the length, in octets
	 */
	virtual size_t length() const = 0;

	/** Make hash from algorithm name.
	 * @param name the algorithm name
	 * @return the resulting hash object
	 */
	static std::unique_ptr<hash> make(const std::string& name);
};

} /* namespace horace */

#endif
