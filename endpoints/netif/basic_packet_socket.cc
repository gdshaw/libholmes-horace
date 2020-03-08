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
	if (lsmonitor) {
		lsmonitor->attach(*this);
	}
}

basic_packet_socket::~basic_packet_socket() {
	if (lsmonitor) {
		lsmonitor->detach(*this);
	}
}

void basic_packet_socket::_update_stats() {
	// The structure tpacket_stats_v3 is identical to
	// tpacket_stats_v2 except that it contains an additional
	// field, which is not needed in this instance.
	std::unique_lock lock(_mutex);
	struct tpacket_stats_v3 stats;
	getsockopt(SOL_PACKET, PACKET_STATISTICS, stats);
	_packets += stats.tp_packets - stats.tp_drops;
	_drops += stats.tp_drops;
}

void basic_packet_socket::measure() {
	_update_stats();
	lsc.record_clock_state(_packets);
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

unsigned int basic_packet_socket::drops() {
	_update_stats();
	std::unique_lock lock(_mutex);
	unsigned int drops = _drops;
	_drops = 0;
	return drops;
}

} /* namespace horace */
