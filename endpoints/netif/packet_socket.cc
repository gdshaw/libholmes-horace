// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/record.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/packet_ref_attribute.h"

#include "packet_socket.h"

namespace horace {

packet_socket::packet_socket(size_t snaplen):
	_builder(record::REC_PACKET) {

	// Enable timestamp generation.
	setsockopt<int>(SOL_SOCKET, SO_TIMESTAMPNS, 1);

	// Initialse buffers for recvmsg.
	_snaplen = snaplen;
	_buffer = std::make_unique<char[]>(snaplen);
	size_t control_size = CMSG_SPACE(sizeof(struct timespec));
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
}

const record& packet_socket::read() {
	// Read a packet from the packet socket, with the MSG_TRUNC flag set
	// in order to return the original length of the packet as opposed to
	// the (potentially truncated) captured length.
	size_t count = recvmsg(&_message, MSG_TRUNC);

	// If the packet was not truncated then the captured length will be
	// equal to the original length, otherwise it will be equal to the
	// configured snaplen.
	size_t pkt_snaplen = (_message.msg_flags & MSG_TRUNC) ?
		_snaplen : count;

	// Find the timestamp which should be present in the control buffer.
	struct timespec* ts = 0;
	for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&_message); cmsg;
		cmsg = CMSG_NXTHDR(&_message, cmsg)) {
			if ((cmsg->cmsg_level == SOL_SOCKET) &&
			(cmsg->cmsg_type == SO_TIMESTAMPNS)) {
				ts = (struct timespec*)CMSG_DATA(cmsg);
			break;
		}
	}

	_builder.reset();
	_builder.append(std::make_shared<posix_timespec_attribute>(*ts));
	_builder.append(std::make_shared<packet_ref_attribute>(
		_buffer.get(), count));
	return _builder;
}

} /* namespace horace */
