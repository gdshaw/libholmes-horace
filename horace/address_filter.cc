// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <arpa/inet.h>

#include "horace/address_filter.h"

namespace horace {

static uint32_t ipv6_mask(unsigned int index, unsigned int prefix_length) {
	if (index >= prefix_length) {
		return 0;
	}
	if (index + 32 <= prefix_length) {
		return -1;
	}
	return -(1UL << (index + 32 - prefix_length));
}

void address_filter::_compile() const {
	// Currently each netblock is tested in isolation. This is tolerable,
	// because it is expected that there will typically only be one
	// netblock to exclude. Optimisation would be possible in the case
	// where there is more than one netblock by fetching each of the
	// two addresses once from the packet and reusing them. The netblocks
	// would then be tested in descending order of prefix length. It
	// would additionally be possible to consolidate the return
	// instructions.

	// In the current implementation, each netblock requires 4
	// instructions to test the source address and 4 to test the
	// destination address. In addition to this, 3 instructions are
	// needed for initially testing the EtherType, and 1 instruction
	// to return if none of the netblocks match.
	uint32_t inet4_count = 1;
	for (auto&& nb : _inet4_netblocks) {
		inet4_count += 6;
		if (nb.prefix_length() != 32) {
			inet4_count += 2;
		}
	}
	uint32_t inet6_count = _inet6_netblocks.size() * 26 + 1;
	uint32_t count = 4 + inet4_count + inet6_count;
	if (1 + inet4_count > 0xff) {
		throw std::invalid_argument("too many addresses in filter");
	}

	_filter = std::make_unique<sock_filter[]>(count);
	unsigned int idx = 0;

	// If ethertype != 0x0800 then accept frame.
	_filter[idx++] = { BPF_LD | BPF_H | BPF_ABS, 0, 0, 12 };
	_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 2, 0, 0x0800 };
	_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, static_cast<uint8_t>(1 + inet4_count), 0, 0x86dd };
	_filter[idx++] = { BPF_RET, 0, 0, 0xffffffff };

	for (auto&& nb : _inet4_netblocks) {
		// If IPv4 source address matches prefix then reject frame.
		uint32_t prefix = *reinterpret_cast<const uint32_t*>(nb.prefix());
		_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 26 };
		if (nb.prefix_length() != 32) {
			uint32_t mask = -(1UL << (32 - nb.prefix_length()));
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, mask };
		}
		_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, 1, htonl(prefix) };
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}
	for (auto&& nb : _inet4_netblocks) {
		// If IPv4 destination address matches prefix then reject frame.
		uint32_t prefix = *reinterpret_cast<const uint32_t*>(nb.prefix());
		_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 30 };
		if (nb.prefix_length() != 32) {
			uint32_t mask = -(1UL << (32 - nb.prefix_length()));
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, mask };
		}
		_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, 1, htonl(prefix) };
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}

	// If IPv4 frame not already rejected then accept it.
	_filter[idx++] = { BPF_RET, 0, 0, 0xffffffff };

	for (auto&& nb : _inet6_netblocks) {
		// If IPv6 source address matches prefix then reject frame.
		const uint32_t* prefix = reinterpret_cast<const uint32_t*>(nb.prefix());
		for (uint32_t i = 0; i != 4; ++i) {
			_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 22 + i * 4 };
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, ipv6_mask(i * 32, nb.prefix_length()) };
			_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, static_cast<uint8_t>(10 - 3 * i), htonl(prefix[i]) };
		}
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}

	for (auto&& nb : _inet6_netblocks) {
		// If IPv6 destination address matches prefix then reject frame.
		const uint32_t* prefix = reinterpret_cast<const uint32_t*>(nb.prefix());
		for (uint32_t i = 0; i != 4; ++i) {
			_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 38 + i * 4 };
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, ipv6_mask(i * 32, nb.prefix_length()) };
			_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, static_cast<uint8_t>(10 - 3 * i), htonl(prefix[i]) };
		}
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}

	// If IPv4 frame not already rejected then accept it.
	_filter[idx++] = { BPF_RET, 0, 0, 0xffffffff };

	_fprog.len = idx;
	_fprog.filter = _filter.get();
}

address_filter::address_filter():
	_modified(true),
	_fprog({0}) {}

void address_filter::add(const inet4_netblock& nb) {
	_inet4_netblocks.push_back(nb);
}

void address_filter::add(const inet6_netblock& nb) {
	_inet6_netblocks.push_back(nb);
}

bool address_filter::empty() const {
	return _inet4_netblocks.empty();
}

const sock_fprog* address_filter::compile() const {
	if (_modified) {
		_compile();
		_modified = false;
	}
	return &_fprog;
}

} /* namespace horace */
