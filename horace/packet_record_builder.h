// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_RECORD_BUILDER
#define LIBHOLMES_HORACE_PACKET_RECORD_BUILDER

#include <memory>
#include <cstdint>

#include "horace/posix_timespec_attribute.h"
#include "horace/packet_ref_attribute.h"
#include "horace/packet_length_attribute.h"
#include "horace/repeat_attribute.h"
#include "horace/record.h"
#include "horace/record_builder.h"

namespace horace {

/** A class for building HORACE packet records. */
class packet_record_builder {
private:
	/** The timestamp attribute. */
	posix_timespec_attribute _ts_attr;

	/** The packet attribute. */
	packet_ref_attribute _pkt_attr;

	/** The packet length attribute. */
	packet_length_attribute _origlen_attr;

	/** The repeat count attribute. */
	repeat_attribute _rpt_attr;

	/** The record buffer.
	 * Records are returned in reverse order.
	 */
	std::vector<record_builder> _buffer;

	/** The number of remaining records in the buffer. */
	unsigned int _count;
public:
	/** Construct empty packet record builder. */
	packet_record_builder();

	/** Build packet record, with optional dropped packets.
	 * If drop_diff is non-zero then two records are built: one for
	 * the missing packet(s) and one for the captured packet.
	 * Otherwise, only one record is built.
	 * @param ts the timestamp, or 0 if unavailable
	 * @param content the packet content
	 * @param snaplen the captured length of the packet
	 * @param origlen the original length of the packet
	 * @param dropped the number of packets dropped
	 */
	void build_packet(const struct timespec* ts, const void* content,
		size_t snaplen, size_t origlen, unsigned int dropped);

	/** Build for dropped packets only.
	 * @param ts the timestamp, or 0 if unavailable
	 * @param dropped the number of packets dropped
	 */
	void build_dropped(const struct timespec* ts, unsigned int dropped);

	/** Get the next available record.
	 * Note that for efficiency, this will take the form of a
	 * record_builder as opposed to a packet_record.
	 * @return the next record, or null if none
	 */
	const record* next();
};

} /* namespace horace */

#endif
