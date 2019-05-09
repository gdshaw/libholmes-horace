// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RING_BUFFER_V2
#define LIBHOLMES_HORACE_RING_BUFFER_V2

#include <poll.h>
#include <linux/if_packet.h>

#include "horace/record_builder.h"

#include "basic_packet_socket.h"

namespace horace {

/** A class to represent an AF_PACKET socket with a TPACKET_V2 ring buffer. */
class ring_buffer_v2:
	public basic_packet_socket {
private:
	/** The mapped ring buffer. */
	char* _rx_ring;

	/** The requested ring buffer geometry. */
	struct tpacket_req _tpreq;

	/** The number of frames within each ring buffer block.
	 * This must be consistent with the geometry specified by _tpreq.
	 */
	int _frames_per_block;

	/** The index of the next frame to be read. */
	int _frame_idx;

	/** The tpacket2_hdr structure for the last frame to be read. */
	struct tpacket2_hdr* _last_tphdr;

	/** A record builder for returning packet records. */
	record_builder _builder;
public:
	/** Create an AF_PACKET socket with a ring buffer.
	 * @param snaplen the required link layer snaplen, in octets
	 * @param buffer_size the required ring buffer size, in octets
	 */
	ring_buffer_v2(size_t snaplen, size_t buffer_size);

	virtual const record& read();
};

} /* namespace horace */

#endif
