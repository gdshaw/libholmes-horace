// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/record.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/packet_ref_attribute.h"
#include "horace/packet_length_attribute.h"
#include "horace/repeat_attribute.h"

#include "packet_socket.h"

namespace horace {

packet_socket::packet_socket(size_t snaplen):
	_pbuilder(record::REC_PACKET),
	_dbuilder(record::REC_PACKET) {

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
	// If a dropped packet count has been returned then there must be
	// a substantive packet waiting to be returned after it.
	if (!_dbuilder.empty()) {
		_dbuilder.reset();
		return _pbuilder;
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

	// Find the timestamp which should be present in the control buffer.
	struct timespec* ts = 0;
	uint32_t drop_diff = 0;
	for (struct cmsghdr* cmsg = CMSG_FIRSTHDR(&message); cmsg;
		cmsg = CMSG_NXTHDR(&message, cmsg)) {

		if (cmsg->cmsg_level == SOL_SOCKET) {
			if (cmsg->cmsg_type == SO_TIMESTAMPNS) {
				ts = reinterpret_cast<struct timespec*>(CMSG_DATA(cmsg));
			} else if (cmsg->cmsg_type == SO_RXQ_OVFL) {
				uint32_t drop_count =
					*reinterpret_cast<uint32_t*>(CMSG_DATA(cmsg));
				drop_diff = drop_count - _drop_count;
				_drop_count = drop_count;
			}
		}
	}

	std::shared_ptr<posix_timespec_attribute> ts_attr =
		std::make_shared<posix_timespec_attribute>(*ts);

	// The substantive packet record is always built at this point while
	// the required data is available.
	_pbuilder.reset();
	_pbuilder.append(ts_attr);
	_pbuilder.append(std::make_shared<packet_ref_attribute>(
		_buffer.get(), pkt_snaplen));
	if (pkt_snaplen != pkt_origlen) {
		_pbuilder.append(std::make_shared<packet_length_attribute>(
			pkt_origlen));
	}

	// However, if a non-zero number of dropped packets have been detected
	// a dropped packet record is constructed too, and returned before
	// the substantive packet record.
	if (drop_diff != 0) {
		_dbuilder.reset();
		_dbuilder.append(ts_attr);
		_dbuilder.append(std::make_shared<repeat_attribute>(drop_diff));
		return _dbuilder;
	}

	// In the absence of any packet drops, the substantive packet record
	// can be returned immediately.
	return _pbuilder;
}

} /* namespace horace */
