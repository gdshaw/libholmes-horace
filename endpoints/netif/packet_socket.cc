// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <climits>

#include "horace/endpoint_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/packet_record_builder.h"

#include "packet_socket.h"

namespace horace {

packet_socket::packet_socket(packet_record_builder& builder, size_t snaplen,
	size_t buffer_size):
	_builder(&builder) {

	// Set receive buffer size.
	if (buffer_size > INT_MAX) {
		throw endpoint_error("receive buffer size too large");
	}
	setsockopt<int>(SOL_SOCKET, SO_RCVBUFFORCE,
		static_cast<int>(buffer_size));

	// Enable timestamp generation.
	setsockopt<int>(SOL_SOCKET, SO_TIMESTAMPNS, 1);

	// Enable dropped packet counter.
	setsockopt<int>(SOL_SOCKET, SO_RXQ_OVFL, 1);

	// Initialse buffers for recvmsg.
	_snaplen = snaplen;
	_buffer = std::make_unique<char[]>(snaplen);
	size_t control_size = CMSG_SPACE(sizeof(struct timespec)) +
		CMSG_SPACE(sizeof(uint32_t));
	_control = std::make_unique<char[]>(control_size);
	_iov[0].iov_base = _buffer.get();
	_iov[0].iov_len = snaplen;

	// Initialise message structure for recvmsg.
	_message.msg_name = &_src_addr;
	_message.msg_namelen = sizeof(_src_addr);
	_message.msg_iov = _iov;
	_message.msg_iovlen = 1;
	_message.msg_control = _control.get();
	_message.msg_controllen = control_size;

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "opened packet socket (" <<
			"snaplen=" << snaplen << ")";
	}
}

const record& packet_socket::read() {
	// If there are any records remaining from the last call to this
	// function then return the next one.
	if (const record* rec = _builder->next()) {
		return *rec;
	}

	// Read a packet from the packet socket, with the MSG_TRUNC flag set
	// in order to return the original length of the packet as opposed to
	// the (potentially truncated) captured length.
	struct msghdr message = _message;
	size_t pkt_origlen = recvmsg(&message, MSG_TRUNC);

	// If the packet was not truncated then the captured length will be
	// equal to the original length, otherwise it will be equal to the
	// configured snaplen.
	size_t pkt_snaplen = (message.msg_flags & MSG_TRUNC) ?
		_snaplen : pkt_origlen;

	// Scan the control buffer, extracting the timestamp (which should
	// always be present) and the drop count (which may be present).
	struct timespec* ts = 0;
	uint32_t drop_diff = 0;
	for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&message); cmsg;
		cmsg = CMSG_NXTHDR(&message, cmsg)) {

		if (cmsg->cmsg_level == SOL_SOCKET) {
			if (cmsg->cmsg_type == SO_TIMESTAMPNS) {
				ts = reinterpret_cast<struct timespec*>(CMSG_DATA(cmsg));
			} else if (cmsg->cmsg_type == SO_RXQ_OVFL) {
				uint32_t new_drop_count =
					*reinterpret_cast<uint32_t*>(CMSG_DATA(cmsg));
				drop_diff = new_drop_count - _drop_count;
				_drop_count = new_drop_count;
			}
		}
	}

        // Correct timestamp.
        if (detect_leap_seconds) {
                lsc.correct(*ts);
        }

	_builder->build_packet(ts, _buffer.get(), pkt_snaplen, pkt_origlen,
		drop_diff);
	return *_builder->next();
}

const std::string& packet_socket::method() const {
	static const std::string name("packet");
	return name;
}

} /* namespace horace */
