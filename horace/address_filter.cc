// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <arpa/inet.h>

#include "horace/address_filter.h"

namespace horace {

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
	unsigned int count = 4 + _inet4_netblocks.size() * 8;
	_filter = std::make_unique<sock_filter[]>(count);
	unsigned int idx = 0;

	// If ethertype != 0x0800 then accept frame.
	_filter[idx++] = { BPF_LD | BPF_H | BPF_ABS, 0, 0, 12 };
	_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 1, 0, 0x0800 };
	_filter[idx++] = { BPF_RET, 0, 0, 0xffffffff };

	for (std::list<inet4_netblock>::const_iterator it = _inet4_netblocks.begin();
		it != _inet4_netblocks.end(); ++it) {

		// If source address matches prefix then reject frame.
		uint32_t prefix = *reinterpret_cast<const uint32_t*>(it->prefix());
		_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 26 };
		if (it->prefix_length() != 32) {
			uint32_t mask = -(1UL << (32 - it->prefix_length()));
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, mask };
		}
		_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, 1, htonl(prefix) };
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}
	for (std::list<inet4_netblock>::const_iterator it = _inet4_netblocks.begin();
		it != _inet4_netblocks.end(); ++it) {

		// If destination address matches prefix then reject frame.
		uint32_t prefix = *reinterpret_cast<const uint32_t*>(it->prefix());
		_filter[idx++] = { BPF_LD | BPF_W | BPF_ABS, 0, 0, 30 };
		if (it->prefix_length() != 32) {
			uint32_t mask = -(1UL << (32 - it->prefix_length()));
			_filter[idx++] = { BPF_ALU | BPF_AND | BPF_K, 0, 0, mask };
		}
		_filter[idx++] = { BPF_JMP | BPF_K | BPF_JEQ, 0, 1, htonl(prefix) };
		_filter[idx++] = { BPF_RET, 0, 0, 0 };
	}

	// If frame not already rejected then accept it.
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
