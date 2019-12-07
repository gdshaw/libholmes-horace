// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/sha256.h"

namespace horace {

sha256::sha256() {
	SHA256_Init(&_ctx);
}

void sha256::_write_direct(const void* buf, size_t nbyte) {
	SHA256_Update(&_ctx, buf, nbyte);
}

const void* sha256::final() {
	flush();
	SHA256_Final(_hash, &_ctx);
	return _hash;
}

} /* namespace horace */
