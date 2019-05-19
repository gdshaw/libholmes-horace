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

	while (srcidx < srclen) {
		char c = str[srcidx];
		if (c == ':') {
			if (srcidx == 0) {
				// Leading colon only allowed if part of double colon.
				if ((srclen < 2) || (str[srcidx + 1] != ':')) {
					throw std::invalid_argument(
						"invalid empty field in IPv6 address");
				}
			} else if (digits == 0) {
				// Empty field indicates compression, which may only occur
				// once.
				if (zidx == -1) {
					zidx = dstidx;
				} else {
					throw std::invalid_argument(
						"multiple compressed fields in IPv6 address");
				}
			} else {
				// Non-empty field terminated by colon, record value.
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
			break;
		} else if (c == '/') {
			break;
		} else {
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
		// Backtrack to the start of the preceding field, discarding
		// the hexadecimal interpretation of its content.
		srcidx -= digits;
		digits = 0;
		acc = 0;
		int decimal_fields = 0;

		if (dstidx > dstlen - 4) {
			throw std::invalid_argument(
				"too many fields in IPv6 address");
		}
		while (srcidx < srclen) {
			char c = str[srcidx];
			if (c == '.') {
				if (digits == 0) {
					// Empty field not allowed in dotted quad section.
					throw std::invalid_argument(
						"invalid empty field in IPv6 address");
				} else {
					// Non-empty field terminated by full stop,
					// record value.
					if (decimal_fields >= 4) {
						throw std::invalid_argument(
							"too many decimal fields in IPv6 address");
					}
				}
				if (acc >= 0x100) {
					throw std::invalid_argument(
						"decimal value too large in IPv6 address");
				}
				_prefix[dstidx] = acc;
				dstidx += 1;
				decimal_fields += 1;
				acc = 0;
				digits = 0;
			} else if ((c >= '0') && (c <= '9')) {
				acc *= 10;
				acc += c - '0';
				digits += 1;
				if (digits > 3) {
					throw std::invalid_argument(
						"decimal field too long in IPv6 address");
				}
			} else if (c == '/') {
				break;
			} else {
				throw std::invalid_argument(
					"invalid character in IPv6 address");
			}
			srcidx += 1;
		}
		if (digits == 0) {
			// Trailing full stop not allowed.
			throw std::invalid_argument(
				"invalid empty field in IPv6 address");
		} else {
			// Non-empty field at end of decimal address, record value.
			if (decimal_fields >= 4) {
				throw std::invalid_argument(
					"too many decimal fields in IPv6 address");
			}
			if (acc >= 0x100) {
				throw std::invalid_argument(
					"decimal value too large in IPv6 address");
			}
			_prefix[dstidx] = acc;
			dstidx += 1;
			decimal_fields += 1;
			acc = 0;
			digits = 0;
		}
		if (decimal_fields < 4) {
			throw std::invalid_argument(
				"too few decimal fields in IPv6 address");
		}
	} else {
		// IPv6 address which does not end with a dotted-quad IPv4 address.
		if (digits == 0) {
			// Trailing colon only allowed if part of double colon.
			if (zidx != dstidx) {
				throw std::invalid_argument(
					"invalid empty field in IPv6 address");
			}
		} else {
			// Non-empty field at end of hex address, record value.
			if (dstidx >= dstlen) {
				throw std::invalid_argument(
					"too many fields in IPv6 address");
			}
			_prefix[dstidx + 0] = acc >> 8;
			_prefix[dstidx + 1] = acc >> 0;
			dstidx += 2;
		}
	}

	if (zidx == -1) {
		// If no compression then there must have been enough fields to
		// complete the address.
		if (dstidx < dstlen) {
			throw std::invalid_argument("too few fields in IPv6 address");
		}
	} else {
		// Compression must expand to at least one field.
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

	if ((srcidx != srclen) && (str[srcidx] == '/')) {
		srcidx += 1;
		digits = 0;
		acc = 0;
		while (srcidx < srclen) {
			char c = str[srcidx];
			if ((c >= '0') && (c <= '9')) {
				acc *= 10;
				acc += c - '0';
				digits += 1;
				if (digits > 3) {
					throw std::invalid_argument(
						"too many digits in prefix length");
				}
			} else {
				throw std::invalid_argument(
					"invalid character in prefix length");
			}
			srcidx += 1;
		}
		if (digits == 0) {
			throw std::invalid_argument(
				"missing prefix length");
		}
		if (acc > 128) {
			throw std::invalid_argument(
				"invalid prefix length");
		}
		_prefix_length = acc;
	} else {
		_prefix_length = 128;
	}
	if (srcidx != srclen) {
		throw std::invalid_argument("invalid IPv6 netmask");
	}
}

} /* namespace horace */
