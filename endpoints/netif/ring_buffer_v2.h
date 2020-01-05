// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RING_BUFFER_V2
#define LIBHOLMES_HORACE_RING_BUFFER_V2

#include <linux/if_packet.h>

#include "basic_packet_socket.h"

namespace horace {

class packet_record_builder;

/** A class to represent an AF_PACKET socket with a TPACKET_V2 ring buffer. */
class ring_buffer_v2:
	public basic_packet_socket {
private:
	/** The mapped ring buffer. */
	volatile char* _rx_ring;

	/** The requested ring buffer geometry. */
	struct tpacket_req _tpreq;

	/** The number of frames within each ring buffer block.
	 * This must be consistent with the geometry specified by _tpreq.
	 */
	int _frames_per_block;

	/** The index of the next frame to be read. */
	int _frame_idx;

	/** The tpacket2_hdr structure for the most recently read frame,
	 * or 0 if none or already released back to the kernel. */
	volatile struct tpacket2_hdr* _last_tphdr;

	/** A builder for making packet records. */
	packet_record_builder* _builder;
public:
	/** Create an AF_PACKET socket with a ring buffer.
	 * @param builder a builder for making packet records
	 * @param snaplen the required link layer snaplen, in octets
	 * @param buffer_size the required ring buffer size, in octets
	 */
	ring_buffer_v2(packet_record_builder& builder, size_t snaplen,
		size_t buffer_size);

	virtual const record& read();
	virtual const std::string& method() const;
};

} /* namespace horace */

#endif
