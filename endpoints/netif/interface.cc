// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <net/if.h>

#include "horace/libc_error.h"

#include "interface.h"

namespace horace {

interface::interface():
	_ifindex(0) {}

interface::interface(const std::string& ifname) {
	_ifindex = if_nametoindex(ifname.c_str());
	if (_ifindex == 0) {
		throw libc_error();
	}
}

} /* namespace horace */
