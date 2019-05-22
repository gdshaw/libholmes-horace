// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RING_BUFFER_V3
#define LIBHOLMES_HORACE_RING_BUFFER_V3

#include <poll.h>
#include <linux/if_packet.h>

#include "horace/record_builder.h"

#include "basic_packet_socket.h"

namespace horace {

/** A class to represent an AF_PACKET socket with a TPACKET_V3 ring buffer. */
class ring_buffer_v3:
	public basic_packet_socket {
private:
	/** The mapped ring buffer. */
	char* _rx_ring;

	/** The requested ring buffer geometry. */
	struct tpacket_req3 _tpreq;

	/** The index of the next block to be read. */
	int _block_idx;

	/** The index of the next frame to be read. */
	int _frame_idx;

	/** The previous block, or 0 if none or already released. */
	struct tpacket_block_desc* _last_block;

	/** The current block. */
	struct tpacket_block_desc* _block;

	/** The current frame, or 0 if waiting for a block. */
	struct tpacket3_hdr* _frame;

	/** A record builder for returning packet records. */
	record_builder _builder;
public:
	/** Create an AF_PACKET socket with a ring buffer.
	 * @param snaplen the required link layer snaplen, in octets
	 * @param buffer_size the required ring buffer size, in octets
	 */
	ring_buffer_v3(size_t snaplen, size_t buffer_size);

	virtual const record& read();
	virtual unsigned int drops() const;
};

} /* namespace horace */

#endif
