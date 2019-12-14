// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <cstring>
#include <string>
#include <iostream>

#include <getopt.h>

#include <sodium.h>

/** Print help text.
 * @param out the ostream to which the help text should be written
 */
void write_help(std::ostream& out) {
	out << "Usage: horace genkey [<options>]" << std::endl;
	out << std::endl;
	out << "Options:" << std::endl;
	out << std::endl;
	out << "  -A  select digital signature algorithm" << std::endl;
	out << "  -h  display this help text then exit" << std::endl;
}

int main(int argc, char* argv[]) {
	// Initialise default options.
	const char* signing_algorithm = 0;

	// Parse command line options.
	int opt;
	while ((opt = getopt(argc, argv, "A:h")) != -1) {
		switch (opt) {
		case 'A':
			signing_algorithm = optarg;
			break;
		case 'h':
			write_help(std::cout);
			return 0;
		}
	}

	// Validate algorithm name.
	if (!signing_algorithm) {
		signing_algorithm = "ed25519";
	}
	if (strcmp(signing_algorithm, "ed25519")) {
		std::cerr << "signing algorithm not recognised" << std::endl;
		exit(1);
	}

	// Generate key.
	unsigned char pk[crypto_sign_PUBLICKEYBYTES];
	unsigned char sk[crypto_sign_SECRETKEYBYTES];
	crypto_sign_keypair(pk, sk);

	// Write key to std::cout in keyfile format.
	char pk_hex[crypto_sign_PUBLICKEYBYTES*2+1];
	char sk_hex[crypto_sign_SECRETKEYBYTES*2+1];
	std::cout << "ed25519:";
	std::cout << sodium_bin2hex(pk_hex, sizeof(pk_hex), pk, sizeof(pk)) << ":";
	std::cout << sodium_bin2hex(sk_hex, sizeof(sk_hex), sk, sizeof(sk)) << std::endl;
}
