// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/sha512_hash.h"

namespace horace {

sha512_hash::sha512_hash() {
	crypto_hash_sha512_init(&_state);
}

void sha512_hash::_write_direct(const void* buf, size_t nbyte) {
	crypto_hash_sha512_update(&_state,
		static_cast<const unsigned char*>(buf), nbyte);
}

const void* sha512_hash::final() {
	flush();
	crypto_hash_sha512_final(&_state, _hash);
	crypto_hash_sha512_init(&_state);
	return _hash;
}

const std::string& sha512_hash::algorithm() const {
        static const std::string name("sha512");
        return name;
}

} /* namespace horace */
