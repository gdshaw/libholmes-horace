// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ED25519_KEYPAIR
#define LIBHOLMES_HORACE_ED25519_KEYPAIR

#include <string>

#include "horace/keypair.h"

namespace horace {

/** A class to represent an ed25519 public/private key pair. */
class ed25519_keypair:
	public keypair {
private:
	/** The public key. */
	std::basic_string<unsigned char> _pk;

	/** The secret key. */
	std::basic_string<unsigned char> _sk;
public:
	/** Construct keypair from keyfile content.
	 * @param content the content of the keyfile
	 */
	explicit ed25519_keypair(const std::string& content);

	virtual std::string sign(const void* content, size_t nbyte) const;
	virtual const std::string& algorithm() const;
	virtual const std::basic_string<unsigned char>& public_key() const;
	virtual const std::basic_string<unsigned char>& secret_key() const;
};

} /* namespace horace */

#endif
