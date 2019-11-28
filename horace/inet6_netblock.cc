// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <stdexcept>

#include "horace/inet6_netblock.h"

namespace horace {

inet6_netblock::inet6_netblock(const std::string& str) {
	size_t srcidx = 0;
	size_t dstidx = 0;
	size_t srclen = str.length();
	size_t dstlen = sizeof(_prefix);
	ssize_t zidx = -1;
	unsigned int digits = 0;
	unsigned int acc = 0;

	// Iterate over the source string, parsing colon-separated fields as
	// hexadecimal address components or compression markers.
	while (srcidx < srclen) {
		char c = str[srcidx];
		if (c == ':') {
			if (srcidx == 0) {
				// Found a a leading colon. This is only allowed as part
				// of a double colon, so check that is the case.
				if ((srclen < 2) || (str[srcidx + 1] != ':')) {
					throw std::invalid_argument(
						"invalid empty field in IPv6 address");
				}
			} else if (digits == 0) {
				// Found an empty field, indicating compression. This may
				// only occur once, so check that it has not been seen
				// previously. If it has not then record its location.
				if (zidx == -1) {
					zidx = dstidx;
				} else {
					throw std::invalid_argument(
						"multiple compressed fields in IPv6 address");
				}
			} else {
				// Reached the end of a non-empty field. Record the field
				// content as a hexadecimal address component, after first
				// checking that this will not overflow the address buffer.
				if (dstidx >= dstlen) {
					throw std::invalid_argument(
						"too many fields in IPv6 address");
				}
				_prefix[dstidx + 0] = acc >> 8;
				_prefix[dstidx + 1] = acc >> 0;
				dstidx += 2;
				acc = 0;
				digits = 0;
			}
		} else if (c == '.') {
			// Found a full stop character. This can only occur
			// when the final four bytes of the address have been
			// written in dotted decimal format. It follows that
			// all hexadecimal address components have been parsed,
			// so break out of the loop.
			break;
		} else if (c == '/') {
			// Found a slash character. This marks the end of the
			// address and the start of the prefix length, so break
			// out of the loop.
			break;
		} else {
			// The only other type of character permitted here is a
			// hex digit (which, if alphabetic, may be upper or lower
			// case). If that is the case then incorporate its value
			// into the current address component. Anything else is
			// an error.
			if ((c >= '0') && (c <= '9')) {
				acc <<= 4;
				acc += c - '0';
			} else if ((c >= 'A') && (c <= 'F')) {
				acc <<= 4;
				acc += c - 'A' + 0xa;
			} else if ((c >= 'a') && (c <= 'f')) {
				acc <<= 4;
				acc += c - 'a' + 0xa;
			} else {
				throw std::invalid_argument(
					"invalid character in IPv6 address");
			}
			digits += 1;
			if (digits > 4) {
				throw std::invalid_argument(
					"field too long in IPv6 address");
			}
		}
		srcidx += 1;
	}

	if ((srcidx != srclen) && (str[srcidx] == '.')) {
		// The initial loop was terminated by a full stop character.
		// Switch to parsing components in dotted decimal notation.

		// The parser will have mistakenly parsed the most recent
		// field as a hexadecimal address component. Backtrack to
		// the start of that field, discarding the hexadecimal
		// result, so that it can be re-parsed as decimal.
		srcidx -= digits;
		digits = 0;
		acc = 0;
		int decimal_fields = 0;

		// Check that the expected number of decimal fields (four)
		// will not overflow the address buffer.
		if (dstidx > dstlen - 4) {
			throw std::invalid_argument(
				"too many fields in IPv6 address");
		}

		// Continue to iterate over the source string, but now parsing
		// dot-separated fields as decimal address components.
		while (srcidx < srclen) {
			char c = str[srcidx];
			if (c == '.') {
				// Reached the end of a field.
				if (digits == 0) {
					// The field is empty. This is not allowed in
					// the dotted decimal section.
					throw std::invalid_argument(
						"invalid empty field in IPv6 address");
				}
				// Reached the end of a non-empty field. Record the
				// field content as a decimal address component, after
				// first checking that this would not exceed the
				// required number of fields.
				if (decimal_fields >= 4) {
					throw std::invalid_argument(
						"too many decimal fields in IPv6 address");
				}
				_prefix[dstidx] = acc;
				dstidx += 1;
				decimal_fields += 1;
				acc = 0;
				digits = 0;
			} else if ((c >= '0') && (c <= '9')) {
				// Found a decimal digit. Incorporate its value into the
				// current address component, after first checking that it
				// does not follow a leading zero.
				if ((digits != 0) && (acc == 0)) {
					throw std::invalid_argument(
						"decimal value with leading zero in IPv6 address");
				}
				acc *= 10;
				acc += c - '0';
				digits += 1;
				if (acc > 255) {
					throw std::invalid_argument(
						"decimal value too large in IPv6 address");
				}
			} else if (c == '/') {
				// Found a slash character. This marks the end of the
				// address and the start of the prefix length, so break
				// out of the loop.
				break;
			} else {
				// Any other character is invalid.
				throw std::invalid_argument(
					"invalid character in IPv6 address");
			}
			srcidx += 1;
		}
		if (digits == 0) {
			// The address has ended with an empty field, which is not
			// allowed.
			throw std::invalid_argument(
				"invalid empty field in IPv6 address");
		} else {
			// The address has ended with a non-empty field. Record the
			// field content as a decimal address component, after first
			// first checking that this would not exceed the required
			// number of fields.
			if (decimal_fields >= 4) {
				throw std::invalid_argument(
					"too many decimal fields in IPv6 address");
			}
			_prefix[dstidx] = acc;
			dstidx += 1;
			decimal_fields += 1;
			acc = 0;
			digits = 0;
		}
		// If decimal fields are present then there must be exactly
		// four. If there are too many then that will already have
		// been detected, however there is a need to check also that
		// there are not too few.
		if (decimal_fields < 4) {
			throw std::invalid_argument(
				"too few decimal fields in IPv6 address");
		}
	} else {
		// The initial loop was terminated by a slash character or the
		// end of the string (no dotted decimal address components).
		if (digits == 0) {
			// The address ended with a trailing colon. This is
			// only allowed as part of a double colon, so check
			// that is the case.
			if (zidx != dstidx) {
				throw std::invalid_argument(
					"invalid empty field in IPv6 address");
			}
		} else {
			// The address ended with a non-empty field. Record the field
			// content as a hexadecimal address component, after first
			// checking that this will not overflow the address buffer.
			if (dstidx >= dstlen) {
				throw std::invalid_argument(
					"too many fields in IPv6 address");
			}
			_prefix[dstidx + 0] = acc >> 8;
			_prefix[dstidx + 1] = acc >> 0;
			dstidx += 2;
			acc = 0;
			digits = 0;
		}
	}

	if (zidx == -1) {
		// No compression marker was observed.
		// Check that there are enough fields to complete the address.
		if (dstidx < dstlen) {
			throw std::invalid_argument("too few fields in IPv6 address");
		}
	} else {
		// A compression marker was observed.
		// Check that there is at least one missing field for it to
		// expand into.
		if (dstidx >= dstlen) {
			throw std::invalid_argument(
				"too many fields in IPv6 address");
		}

		// Decompress by moving everything to the right of the double
		// colon to the end of the address, then filling with zeros.
		int offset = dstlen - dstidx;
		for (int i = dstidx - 1; i >= zidx; --i) {
			_prefix[i + offset] = _prefix[i];
		}
		for (int i = zidx + offset - 1; i >= zidx; --i) {
			_prefix[i] = 0;
		}
	}

	// Parse optional prefix length.
	if ((srcidx != srclen) && (str[srcidx] == '/')) {
		// Found slash character, therefore prefix length is expected.
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
						"prefix value with leading zero in IPv6 netblock");
				}
				acc *= 10;
				acc += c - '0';
				digits += 1;
				if (acc > 128) {
					throw std::invalid_argument(
						"prefix length too large in IPv6 netblock");
				}
			} else {
				// Any other character is invalid.
				throw std::invalid_argument(
					"invalid character in prefix length");
			}
			srcidx += 1;
		}
		if (digits == 0) {
			throw std::invalid_argument(
				"missing prefix length in IPv6 netblock");
		}
		_prefix_length = acc;
	} else {
		// No prefix length specified, so default to /128.
		_prefix_length = 128;
	}

	// Check that there are no trailing characters.
	if (srcidx != srclen) {
		throw std::invalid_argument("invalid character in IPv6 netblock");
	}
}

} /* namespace horace */
