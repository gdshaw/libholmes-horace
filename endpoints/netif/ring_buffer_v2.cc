// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <poll.h>
#include <net/ethernet.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include "horace/endpoint_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/libc_error.h"
#include "horace/packet_record_builder.h"

#include "ring_buffer_v2.h"

namespace horace {

ring_buffer_v2::ring_buffer_v2(packet_record_builder& builder, size_t snaplen,
	size_t buffer_size):
	_builder(&builder),
	_tpreq({0}),
	_frame_idx(0),
	_last_tphdr(0) {

	// Select ring buffer protocol version.
	setsockopt<int>(SOL_PACKET, PACKET_VERSION, TPACKET_V2);

	// Calculate ring buffer geometry.
	// The frame size is determined by the snaplen.
	// The block size is constrained to be a power-of-two multiple of
	// the virtual memory page size, large enough to hold at least 3
	// frames.
	_tpreq.tp_frame_size = TPACKET_ALIGN(TPACKET2_HDRLEN + ETH_HLEN) +
		TPACKET_ALIGN(snaplen - ETH_HLEN);
	_tpreq.tp_block_size = sysconf(_SC_PAGESIZE);
	while (_tpreq.tp_block_size < _tpreq.tp_frame_size * 3) {
		_tpreq.tp_block_size <<= 1;
	}
	_frames_per_block = _tpreq.tp_block_size / _tpreq.tp_frame_size;
	_tpreq.tp_block_nr = buffer_size / _tpreq.tp_block_size;
	_tpreq.tp_frame_nr = _tpreq.tp_block_nr * _frames_per_block;

	// Check that the ring buffer contains a non-zero number of frames
	// (and thus also a non-zero number of blocks, and frames per block).
	if (_tpreq.tp_frame_nr == 0) {
		throw endpoint_error(
			"buffer capacity insufficient for snaplen");
	}

	// Create the ring buffer.
	setsockopt(SOL_PACKET, PACKET_RX_RING, _tpreq);

	// Map the ring buffer into virtual memory.
	size_t rx_ring_size = _tpreq.tp_block_nr * _tpreq.tp_block_size;
	_rx_ring = static_cast<char*>(mmap(0, rx_ring_size,
		PROT_READ|PROT_WRITE, MAP_SHARED, *this, 0));

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
	if (const record* rec = _builder->next()) {
		return *rec;
	}

	if (_last_tphdr) {
		_last_tphdr->tp_status = TP_STATUS_KERNEL;
		_last_tphdr = 0;
	}

	size_t buffer_idx = _frame_idx / _frames_per_block;
	size_t frame_idx_diff = _frame_idx % _frames_per_block;

	volatile char* buffer_ptr = _rx_ring + buffer_idx * _tpreq.tp_block_size;
	volatile char* frame_ptr = buffer_ptr + frame_idx_diff * _tpreq.tp_frame_size;
	volatile struct tpacket2_hdr* tphdr = (struct tpacket2_hdr*)frame_ptr;

	while (!(tphdr->tp_status & TP_STATUS_USER)) {
		wait(POLLIN);
	}

	unsigned int drop_count = 0;
	if (tphdr->tp_status & TP_STATUS_LOSING) {
		drop_count = drops();
	}

	// Extract and correct timestamp.
	struct timespec ts;
	ts.tv_sec = tphdr->tp_sec;
	ts.tv_nsec = tphdr->tp_nsec;
	if (detect_leap_seconds) {
		lsc.correct(ts);
	}

	// Note that whilst the ring buffer as a whole is volatile, the
	// packet content should be constant for the lifetime of the
	// pointer that is created here.
	const char* content = const_cast<const char*>(frame_ptr + tphdr->tp_mac);
	size_t pkt_origlen = tphdr->tp_len;
	size_t pkt_snaplen = tphdr->tp_snaplen;

	_last_tphdr = tphdr;
	_frame_idx += 1;
	if (_frame_idx == _tpreq.tp_frame_nr) {
		_frame_idx = 0;
	}

	_builder->build_packet(&ts, content, pkt_snaplen, pkt_origlen,
		drop_count);
	return *_builder->next();
}

const std::string& ring_buffer_v2::method() const {
	static const std::string name("ringv2");
	return name;
}

} /* namespace horace */
