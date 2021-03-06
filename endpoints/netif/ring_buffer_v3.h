// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RING_BUFFER_V3
#define LIBHOLMES_HORACE_RING_BUFFER_V3

#include <linux/if_packet.h>

#include "basic_packet_socket.h"

namespace horace {

class packet_record_builder;

/** A class to represent an AF_PACKET socket with a TPACKET_V3 ring buffer. */
class ring_buffer_v3:
	public basic_packet_socket {
private:
	/** The mapped ring buffer. */
	volatile char* _rx_ring;

	/** The requested ring buffer geometry. */
	struct tpacket_req3 _tpreq;

	/** The index of the next block to be read. */
	int _block_idx;

	/** The index of the next frame to be read. */
	int _frame_idx;

	/** The previous block, or 0 if none or already released
	 * back to the kernel. */
	volatile struct tpacket_block_desc* _last_block;

	/** The current block. */
	volatile struct tpacket_block_desc* _block;

	/** The current frame, or 0 if waiting for a block. */
	volatile struct tpacket3_hdr* _frame;

	/** A builder for making packet records. */
	packet_record_builder* _builder;
public:
	/** Create an AF_PACKET socket with a ring buffer.
	 * @param builder a builder for making packet records
	 * @param snaplen the required link layer snaplen, in octets
	 * @param buffer_size the required ring buffer size, in octets
	 */
	ring_buffer_v3(packet_record_builder& builder, size_t snaplen,
		size_t buffer_size);

	virtual const record& read();
	virtual const std::string& method() const;
};

} /* namespace horace */

#endif
