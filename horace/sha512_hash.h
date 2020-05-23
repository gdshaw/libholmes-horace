// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SHA512_HASH
#define LIBHOLMES_HORACE_SHA512_HASH

#include <sodium.h>

#include "horace/hash.h"

namespace horace {

/** An octet writer class for calculating a SHA512 hash. */
class sha512_hash:
	public hash {
private:
	/** The SHA512 hash context. */
	crypto_hash_sha512_state _state;

	/** The hashed result. */
	unsigned char _hash[64];
protected:
	virtual void _write_direct(const void* buf, size_t nbyte);
public:
	/** Construct SHA512 octet writer. */
	sha512_hash();

	sha512_hash(const sha512_hash& that) = delete;
	sha512_hash& operator=(const sha512_hash& that) = delete;

	virtual const void* final();

	virtual size_t length() const {
		return 64;
	}

	virtual const std::string& algorithm() const;
};

} /* namespace horace */

#endif
