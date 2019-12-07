// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SHA256
#define LIBHOLMES_HORACE_SHA256

#include <openssl/sha.h>

#include "horace/hash.h"

namespace horace {

/** An octet writer class for calculating a SHA256 hash. */
class sha256:
	public hash {
private:
	/** The SHA256 hash context. */
	SHA256_CTX _ctx;

	/** The hashed result. */
	unsigned char _hash[32];
protected:
	virtual void _write_direct(const void* buf, size_t nbyte);
public:
	/** Construct SHA256 octet writer. */
	sha256();

	sha256(const sha256& that) = delete;
	sha256& operator=(const sha256& that) = delete;

	virtual const void* final();

	virtual size_t length() const {
		return 32;
	}
};

} /* namespace horace */

#endif
