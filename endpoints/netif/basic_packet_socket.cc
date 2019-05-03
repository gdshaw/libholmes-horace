// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <netpacket/packet.h>

#include "basic_packet_socket.h"

namespace horace {

basic_packet_socket::basic_packet_socket():
	socket_descriptor(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) {}

void basic_packet_socket::bind(const interface& iface) {
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = iface;
	addr.sll_protocol = htons(ETH_P_ALL);
	socket_descriptor::bind(addr);
}

} /* namespace horace */
