// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/inet4_netblock.h"

namespace horace {

inet4_netblock::inet4_netblock(const std::string& str) {
	size_t srcidx = 0;
	size_t dstidx = 0;
	size_t srclen = str.length();
	size_t dstlen = sizeof(_prefix);
	unsigned int digits = 0;
	unsigned int acc = 0;

	// Iterate over the source string, parsing dot-separated fields as
	// decimal address components.
	while (srcidx < srclen) {
		char c = str[srcidx];
		if (c == '.') {
			// Reached the end of a field.
			if (digits == 0) {
				// The field is empty, which is not allowed.
				throw std::invalid_argument(
					"invalid empty field in IPv4 address");
			}
			// Reached the end of a non-empty field. Record the
			// field content as a decimal address component, after
			// first checking that this would not exceed the
			// required number of address components.
			if (dstidx >= dstlen) {
				throw std::invalid_argument(
					"too many fields in IPv4 address");
			}
			_prefix[dstidx] = acc;
			dstidx += 1;
			acc = 0;
			digits = 0;
		} else if ((c >= '0') && (c <= '9')) {
			// Found a decimal digit. Incorporate its value into the
			// current address component, after first checking that it
			// does not follow a leading zero.
			if ((digits != 0) && (acc == 0)) {
				throw std::invalid_argument(
					"leading zero in IPv4 address");
			}
			acc *= 10;
			acc += c - '0';
			digits += 1;
			if (acc > 255) {
				throw std::invalid_argument(
					"value too large in IPv4 address");
			}
		} else if (c == '/') {
			// Found a slash character. This marks the end of the
			// address and the start of the prefix length, so break
			// out of the loop.
			break;
		} else {
			// Any other character is invalid.
			throw std::invalid_argument(
				"invalid character in IPv4 netblock");
		}
		srcidx += 1;
	}
	if (digits == 0) {
		// The address has ended with an empty field, which is not
		// allowed.
		throw std::invalid_argument(
			"invalid empty field in IPv4 address");
	}
	// The address has ended with a non-empty field. Record the
	// field content as an address component, after first
	// checking that this would not exceed the required number
	// of address components.
	if (dstidx >= dstlen) {
		throw std::invalid_argument(
			"too many fields in IPv4 address");
	}
	_prefix[dstidx] = acc;
	dstidx += 1;
	acc = 0;
	digits = 0;

	// Exactly four fields are required. If there are too many then that
	// will already have been detected, however there is a need to check
	// also that there are not too few.
	if (dstidx < dstlen) {
		throw std::invalid_argument(
			"too few fields in IPv4 address");
	}

	// Parse optional prefix length.
	if ((srcidx != srclen) && (str[srcidx] == '/')) {
		// Found slash character, therefore prefix length is present.
		srcidx += 1;

		// Iterate over the source string, parsing the prefix length.
		while (srcidx < srclen) {
			char c = str[srcidx];
			if ((c >= '0') && (c <= '9')) {
				// Found a decimal digit. Incorporate its value
				// into the prefix length, after first checking
				// that it does not follow a leading zero.
				if ((digits != 0) && (acc == 0)) {
					throw std::invalid_argument(
						"prefix length with leading zero in IPv4 netblock");
				}
				acc *= 10;
				acc += c - '0';
				digits += 1;
				if (acc > 32) {
					throw std::invalid_argument(
						"prefix length too large in IPv4 netblock");
				}
			} else {
				// Any other character is invalid.
				throw std::invalid_argument(
					"invalid character in IPv4 netblock");
			}
			srcidx += 1;
		}
		if (digits == 0) {
			throw std::invalid_argument(
				"missing prefix length in IPv4 netblock");
		}
		_prefix_length = acc;
	} else {
		// No prefix length specified, so default to /32.
		_prefix_length = 32;
	}

	// Check that there are no trailing characters.
	if (srcidx != srclen) {
		throw std::invalid_argument("invalid character in IPv4 netblock");
	}
}

} /* namespace horace */
