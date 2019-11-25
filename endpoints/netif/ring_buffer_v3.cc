// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <poll.h>
#include <net/ethernet.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include "horace/libc_error.h"
#include "horace/endpoint_error.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/packet_record_builder.h"

#include "ring_buffer_v3.h"

namespace horace {

ring_buffer_v3::ring_buffer_v3(packet_record_builder& builder, size_t snaplen,
	size_t buffer_size):
	_builder(&builder),
	_tpreq({0}),
	_block_idx(0),
	_frame_idx(0),
	_last_block(0),
	_block(0),
	_frame(0) {

	// Select ring buffer protocol version.
	setsockopt<int>(SOL_PACKET, PACKET_VERSION, TPACKET_V3);

	// Frame size is arbitrary for TPACKET_V3, but set based on snaplen
	// as per previous protocol versions to provide basis for setting
	// block size.
	_tpreq.tp_frame_size = TPACKET_ALIGN(TPACKET3_HDRLEN + ETH_HLEN) +
		TPACKET_ALIGN(snaplen - ETH_HLEN);
	_tpreq.tp_block_size = sysconf(_SC_PAGESIZE);
	while (_tpreq.tp_block_size < _tpreq.tp_frame_size * 3) {
		_tpreq.tp_block_size <<= 1;
	}
	size_t frames_per_block = _tpreq.tp_block_size / _tpreq.tp_frame_size;
	_tpreq.tp_block_nr = buffer_size / _tpreq.tp_block_size;
	_tpreq.tp_frame_nr = _tpreq.tp_block_nr * frames_per_block;
	_tpreq.tp_retire_blk_tov = 60;
	_tpreq.tp_feature_req_word = 0;

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
	_block = (tpacket_block_desc*)_rx_ring;

	if (log->enabled(logger::log_info)) {
		log_message msg1(*log, logger::log_info);
		msg1 << "opened packet socket (" <<
			"snaplen=" << snaplen << ")";
	}
	if (log->enabled(logger::log_info)) {
		log_message msg2(*log, logger::log_info);
		msg2 << "ring buffer v3 (" <<
			"bs=" << _tpreq.tp_block_size << ", " <<
			"fs=" << _tpreq.tp_frame_size << ", " <<
			"nb=" << _tpreq.tp_block_nr << ", " <<
			"nf=" << _tpreq.tp_frame_nr << ")";
	}
}

const record& ring_buffer_v3::read() {
	if (const record* rec = _builder->next()) {
		return *rec;
	}

	if (!_frame) {
		// Free last block, if not already freed.
		if (_last_block) {
			_last_block->hdr.bh1.block_status = TP_STATUS_KERNEL;
			_last_block = 0;
		}

		// Wait for the current block to become ready.
		while (!(_block->hdr.bh1.block_status & TP_STATUS_USER)) {
			wait(POLLIN);
		}

		// Confirm that the block is non-empty (which it presumably ought to be).
		if (_block->hdr.bh1.num_pkts == 0) {
			throw std::runtime_error("encountered empty ring buffer block");
		}

		// Calculate address of first frame.
		char* block_ptr = (char*)_block;
		_frame = (struct tpacket3_hdr*)(block_ptr + _block->hdr.bh1.offset_to_first_pkt);

		// Now check for dropped packets.
		if (_block->hdr.bh1.block_status & TP_STATUS_LOSING) {
			// Because further packets may have been buffered since the
			// TP_STATUS_LOSING flag was set, it is possible that the
			// dropped packets have already been reported. For this
			// reason, need to check that the count is non-zero before
			// deciding whether to report it.
			if (unsigned int count = drops()) {
				struct timespec ts;
				if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
					throw libc_error();
				}
				_builder->build_dropped(&ts, count);
				return *_builder->next();
			}
		}
	}

	// Extract required data from frame buffer.
	struct timespec ts;
	ts.tv_sec = _frame->tp_sec;
	ts.tv_nsec = _frame->tp_nsec;
	const char* content = ((const char*)_frame) + _frame->tp_mac;
	size_t pkt_origlen = _frame->tp_len;
	size_t pkt_snaplen = _frame->tp_snaplen;

	// Advance to next frame.
	_frame_idx += 1;
	if (_frame_idx == _block->hdr.bh1.num_pkts) {
		// If no more frames in block then advance to next block, leaving
		// frame set to null.
		_frame_idx = 0;
		_frame = 0;
		_last_block = _block;
		_block_idx += 1;
		if (_block_idx == _tpreq.tp_block_nr) {
			_block_idx = 0;
		}
		_block = (tpacket_block_desc*)(_rx_ring + _block_idx * _tpreq.tp_block_size);
	} else {
		// Otherwise, calculate the address of the next frame.
		char* frame_ptr = (char*)_frame;
		_frame = (struct tpacket3_hdr*)(frame_ptr + _frame->tp_next_offset);
	}

	_builder->build_packet(&ts, content, pkt_snaplen, pkt_origlen, 0);
	return *_builder->next();
}

unsigned int ring_buffer_v3::drops() const {
	struct tpacket_stats_v3 stats;
	getsockopt(SOL_PACKET, PACKET_STATISTICS, stats);
	return stats.tp_drops;
}

} /* namespace horace */
