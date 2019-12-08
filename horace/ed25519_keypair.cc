// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sodium.h>

#include "horace/ed25519_keypair.h"

namespace horace {

ed25519_keypair::ed25519_keypair(const std::string& content) {
	std::string::size_type f1 = content.find(':');
	if (f1 == std::string::npos) {
		throw std::runtime_error("malformed keyfile");
	}
	std::string::size_type f2 = content.find(':', f1 + 1);
	if (f2 == std::string::npos) {
		throw std::runtime_error("malformed keyfile");
	}

	std::string sk_hex = content.substr(f1 + 1, f2 - f1 - 1);
	std::string pk_hex = content.substr(f2 + 1);

	size_t sk_len = 0;
	size_t pk_len = 0;
	if (sodium_hex2bin(_sk, sizeof(_sk),
		sk_hex.data(), sk_hex.length(), "", &sk_len, 0)) {
		throw std::runtime_error("malformed private key");
	}
	if (sk_len != sizeof(_sk)) {
		throw std::runtime_error("undersized private key");
	}
	if (sodium_hex2bin(_pk, sizeof(_pk),
		pk_hex.data(), pk_hex.length(), "", &pk_len, 0)) {
		throw std::runtime_error("malformed public key");
	}
	if (pk_len != sizeof(_pk)) {
		throw std::runtime_error("undersized public key");
	}
}

std::string ed25519_keypair::sign(const void* content, size_t nbyte) const {
	unsigned char sig[64];
	crypto_sign_ed25519_detached(sig, 0,
		static_cast<const unsigned char*>(content), nbyte, _sk);
	return std::string(reinterpret_cast<const char*>(sig), sizeof(sig));
}

} /* namespace horace */
