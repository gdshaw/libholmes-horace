// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/sha256.h"

namespace horace {

sha256::sha256() {
	crypto_hash_sha256_init(&_state);
}

void sha256::_write_direct(const void* buf, size_t nbyte) {
	crypto_hash_sha256_update(&_state,
		static_cast<const unsigned char*>(buf), nbyte);
}

const void* sha256::final() {
	flush();
	crypto_hash_sha256_final(&_state, _hash);
	crypto_hash_sha256_init(&_state);
	return _hash;
}

} /* namespace horace */
