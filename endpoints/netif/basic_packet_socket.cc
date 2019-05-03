// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <arpa/inet.h>
#include <net/ethernet.h>
#include <sys/socket.h>

#include "basic_packet_socket.h"

namespace horace {

basic_packet_socket::basic_packet_socket():
	socket_descriptor(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)) {}

} /* namespace horace */
