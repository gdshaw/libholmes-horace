// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <poll.h>

#include "horace/address_info.h"

#include "ntp_assoc_attr_builder.h"
#include "ntp_attr_builder.h"
#include "ntp_client.h"

namespace horace {

ntp_client::ntp_client():
	_sock(AF_INET, SOCK_DGRAM, 0),
	_seqnum(1) {

	address_info addrinfo("localhost", "123", SOCK_DGRAM, false);
	_sock = addrinfo.make_socket();
	addrinfo.connect(_sock);
}

void ntp_client::send(const ntp_datagram& datagram) {
	const std::basic_string<unsigned char>& content = datagram.content();
	_sock.send(content.data(), content.length());
}

std::unique_ptr<ntp_datagram> ntp_client::recv() {
	if (_sock.wait(POLLIN, 100) & POLLIN) {
		ssize_t count = _sock.recv(_buffer, sizeof(_buffer));
		return std::make_unique<ntp_datagram>(
			std::basic_string<unsigned char>(_buffer, count));
	}
	return std::unique_ptr<ntp_datagram>();
}

void ntp_client::update_peer(uint16_t associd) {
	// Create and record the request and response objects.
	uint16_t seqnum = _seqnum++;
	_requests[seqnum] = std::make_unique<ntp_datagram>(
		2, seqnum, associd);
	_responses[seqnum] = std::make_unique<ntp_response>();

	// Send the request to the server.
	send(*_requests[seqnum]);

	// Receive and process response fragments until either the
	// response to the current request is complete, or a timeout
	// occurs. Any response fragments to previous requests are
	// accepted and processed, but will not terminate the loop.
	while (!_responses[seqnum]->complete()) {
		std::unique_ptr<ntp_datagram> fragment = recv();
		if (fragment) {
			auto f = _requests.find(fragment->seqnum());
			auto g = _responses.find(fragment->seqnum());
			if ((f != _requests.end()) && (g != _responses.end())) {
				if (fragment->responds_to(*f->second)) {
					g->second->add(fragment);
					if (g->second->complete()) {
						_peers[f->second->associd()] =
							std::make_unique<ntp_peer>(*g->second);
					}
				}
			}
		} else {
			break;
		}
	}
}

bool ntp_client::update_peers() {
	_requests.clear();
	_responses.clear();
	_peers.clear();

	// Always report on association ID 0.
	_peers[0] = std::unique_ptr<ntp_peer>();

	ntp_datagram request(1, _seqnum++, 0);
	std::unique_ptr<ntp_datagram> response;
	for (unsigned int retry = 0; retry != 5; ++retry) {
		send(request);
		response = recv();
		if (response) {
			break;
		}
	}

	if (response) {
		std::basic_string<unsigned char> payload = response->payload();
		for (int i = 0; i < payload.length(); i += 4) {
			unsigned int associd = payload[i + 0];
			associd <<= 8;
			associd += payload[i + 1];

			unsigned int status = payload[i + 2];
			status <<= 8;
			status += payload[i + 3];
			_peers[associd] = std::unique_ptr<ntp_peer>();
		}
	}

	// Repeat each request up to 3 times if necessary.
	unsigned int missing = 0;
	for (int i = 0; i != 3; ++i) {
		// Iterate over the peers.
		missing = 0;
		for (auto& j : _peers) {
			// Act only if a response for this peer has not been received yet.
			if (!j.second) {
				update_peer(j.first);
			}
			if (!j.second) {
				missing += 1;
			}
		}
		if (missing == 0) {
			break;
		}
	}
	return (missing == 0);
}

void ntp_client::build(ntp_attr_builder& builder, ntp_assoc_attr_builder& assoc_builder) {
	for (auto& i : _peers) {
		i.second->build(assoc_builder);
		builder.add_assoc_attr(assoc_builder.build());
	}
}

} /* namespace horace */
