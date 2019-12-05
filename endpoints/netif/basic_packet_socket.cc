// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include "linux/if_packet.h"

#include "interface.h"
#include "basic_packet_socket.h"

namespace horace {

basic_packet_socket::basic_packet_socket():
	socket_descriptor(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) {

	interruptible(true);
}

void basic_packet_socket::bind(const interface& iface) {
	struct sockaddr_ll addr = {0};
	addr.sll_family = AF_PACKET;
	addr.sll_ifindex = iface.ifindex();
	addr.sll_protocol = htons(ETH_P_ALL);
	socket_descriptor::bind(addr);
}

void basic_packet_socket::set_promiscuous(const interface& iface) {
	struct packet_mreq mreq = {0};
	mreq.mr_ifindex = iface.ifindex();
	mreq.mr_type = PACKET_MR_PROMISC;
	setsockopt(SOL_PACKET, PACKET_ADD_MEMBERSHIP, mreq);
}

unsigned int basic_packet_socket::drops() const {
	struct tpacket_stats stats;
	getsockopt(SOL_PACKET, PACKET_STATISTICS, stats);
	return stats.tp_drops;
}

} /* namespace horace */
