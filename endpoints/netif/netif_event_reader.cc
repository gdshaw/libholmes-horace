// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "packet_socket.h"
#include "netif_event_reader.h"

namespace horace {

class record;

netif_event_reader::netif_event_reader() {
	_sock = std::make_unique<packet_socket>(1514);
}

const record& netif_event_reader::read() {
	return _sock->read();
};

} /* namespace horace */