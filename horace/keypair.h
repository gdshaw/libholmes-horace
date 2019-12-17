// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_KEYPAIR
#define LIBHOLMES_HORACE_KEYPAIR

#include <memory>
#include <string>

namespace horace {

/** A class to represent a cryptographic public/private key pair. */
class keypair {
public:
	/** Sign a message with this keypair.
	 * @param content the content to be signed
	 * @param nbyte the number of bytes to be signed
	 * @return the signature
	 */
	virtual std::string sign(const void* content, size_t nbyte) const = 0;

	/** Get the algorithm name for this keypair.
	 * @return the algorithm name
	 */
	virtual const std::string& algorithm() const = 0;

	/** Get the public key for this keypair.
	 * @return the public key
	 */
	virtual const std::basic_string<unsigned char>& public_key() const = 0;

	/** Get the secret key for this keypair.
	 * @return the secret key
	 */
	virtual const std::basic_string<unsigned char>& secret_key() const = 0;

	/** Load keypair from file.
	 * The actual type of the object returned will match the
	 * algorithm specified in the file.
	 * @param pathname the pathname of the file
	 * @return the keypair loaded
	 */
	static std::unique_ptr<keypair> load(const std::string& pathname);
};

} /* namespace horace */

#endif
