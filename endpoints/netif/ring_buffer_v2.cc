// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <net/ethernet.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/record.h"
#include "horace/posix_timespec_attribute.h"
#include "horace/packet_ref_attribute.h"
#include "horace/packet_length_attribute.h"
#include "horace/libc_error.h"

#include "ring_buffer_v2.h"

namespace horace {

ring_buffer_v2::ring_buffer_v2(size_t snaplen, size_t buffer_size):
	_tpreq({0}),
	_frame_idx(0),
	_last_tphdr(0),
	_builder(record::REC_PACKET) {

	setsockopt<int>(SOL_PACKET, PACKET_VERSION, TPACKET_V2);

	_tpreq.tp_frame_size = TPACKET_ALIGN(TPACKET2_HDRLEN + ETH_HLEN) +
		TPACKET_ALIGN(snaplen - ETH_HLEN);
	_tpreq.tp_block_size = sysconf(_SC_PAGESIZE);
	while (_tpreq.tp_block_size < _tpreq.tp_frame_size * 3) {
		_tpreq.tp_block_size <<= 1;
	}
	_frames_per_block = _tpreq.tp_block_size / _tpreq.tp_frame_size;
	_tpreq.tp_block_nr = buffer_size / _tpreq.tp_block_size;
	_tpreq.tp_frame_nr = _tpreq.tp_block_nr * _frames_per_block;
	setsockopt(SOL_PACKET, PACKET_RX_RING, _tpreq);

	size_t rx_ring_size = _tpreq.tp_block_nr * _tpreq.tp_block_size;
	_rx_ring = (char*)mmap(0, rx_ring_size, PROT_READ|PROT_WRITE, MAP_SHARED, *this, 0);

	if (log->enabled(logger::log_info)) {
		log_message msg1(*log, logger::log_info);
		msg1 << "opened packet socket (" <<
			"snaplen=" << snaplen << ")";
	}
	if (log->enabled(logger::log_info)) {
		log_message msg2(*log, logger::log_info);
		msg2 << "ring buffer v2 (" <<
			"bs=" << _tpreq.tp_block_size << ", " <<
			"fs=" << _tpreq.tp_frame_size << ", " <<
			"nb=" << _tpreq.tp_block_nr << ", " <<
			"nf=" << _tpreq.tp_frame_nr << ")";
	}
}

const record& ring_buffer_v2::read() {
	if (_last_tphdr) {
		_last_tphdr->tp_status = TP_STATUS_KERNEL;
		_last_tphdr = 0;
	}

	size_t buffer_idx = _frame_idx / _frames_per_block;
	size_t frame_idx_diff = _frame_idx % _frames_per_block;

	char* buffer_ptr = _rx_ring + buffer_idx * _tpreq.tp_block_size;
	char* frame_ptr = buffer_ptr + frame_idx_diff * _tpreq.tp_frame_size;
	struct tpacket2_hdr* tphdr = (struct tpacket2_hdr*)frame_ptr;

	while (!(tphdr->tp_status & TP_STATUS_USER)) {
		wait(POLLIN);
	}

	struct timespec ts;
	ts.tv_sec = tphdr->tp_sec;
	ts.tv_nsec = tphdr->tp_nsec;
	const char* content = frame_ptr + tphdr->tp_mac;
	size_t pkt_origlen = tphdr->tp_len;
	size_t pkt_snaplen = tphdr->tp_snaplen;

	_last_tphdr = tphdr;
	_frame_idx += 1;
	if (_frame_idx == _tpreq.tp_frame_nr) {
		_frame_idx = 0;
	}

	_builder.reset();
	_builder.append(std::make_shared<posix_timespec_attribute>(ts));
	_builder.append(std::make_shared<packet_ref_attribute>(
		content, pkt_snaplen));
	if (pkt_snaplen != pkt_origlen) {
		_builder.append(std::make_shared<packet_length_attribute>(
			pkt_origlen));
	}
	return _builder;
}

} /* namepsace horace */
