// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <net/ethernet.h>

#include "horace/libc_error.h"
#include "horace/socket_descriptor.h"

#include "interface.h"

namespace horace {

interface::interface():
	_ifindex(0),
	_linktype(-1) {}

interface::interface(const std::string& ifname):
	_linktype(-1) {

	// The ioctl calls below require an open socket descriptor.
	socket_descriptor fd(AF_INET, SOCK_DGRAM, 0);

	// Copy the interface name into the ifreq structure, ensuring
	// that it does not overflow the buffer. If an overflow does
	// occur then report it in the same manner as any other
	// non-existent interface name.
	struct ifreq ifr = {0};
	if (snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s",
		ifname.c_str()) >= sizeof(ifr.ifr_name)) {

		throw libc_error(ENODEV);
	}

	// Get the interface index.
	if (ioctl(fd, SIOCGIFINDEX, &ifr) == -1) {
		throw libc_error();
	}
	_ifindex = ifr.ifr_ifindex;

	// Get the hardware address and hardware type.
	if (ioctl(fd, SIOCGIFHWADDR, &ifr) == -1) {
		throw libc_error();
	}

	// Interpret the hardware type. Currently only Ethernet is recognised.
	uint16_t arphrd = ifr.ifr_hwaddr.sa_family;
	if (arphrd == ARPHRD_ETHER) {
		_linktype = linktype_ethernet;
		_hwaddr = std::string(ifr.ifr_hwaddr.sa_data, ETH_ALEN);
	}
}

} /* namespace horace */
