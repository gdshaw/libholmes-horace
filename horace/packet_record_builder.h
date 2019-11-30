// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_PACKET_RECORD_BUILDER
#define LIBHOLMES_HORACE_PACKET_RECORD_BUILDER

#include <vector>

#include "horace/attribute_list.h"
#include "horace/record.h"
#include "horace/timestamp_attribute.h"
#include "horace/binary_ref_attribute.h"
#include "horace/unsigned_integer_attribute.h"

namespace horace {

class record;
class session_builder;

/** A class for building HORACE packet records. */
class packet_record_builder {
private:
	/** The channel number. */
	int _channel;

	/** The timestamp attribute. */
	timestamp_attribute _ts_attr;

	/** The packet attribute. */
	binary_ref_attribute _pkt_attr;

	/** The packet length attribute. */
	unsigned_integer_attribute _origlen_attr;

	/** The repeat count attribute. */
	unsigned_integer_attribute _rpt_attr;

	/** The record buffer.
	 * Records are returned in reverse order.
	 */
	std::vector<record> _buffer;

	/** The number of pending records in the buffer. */
	unsigned int _count;
public:
	/** Construct empty packet record builder.
	 * @param session the applicable session builder
	 * @param channel the applicable channel number
	 */
	packet_record_builder(session_builder& session, int channel);

	/** Build packet record, with optional dropped packets.
	 * If the dropped argument is non-zero then two records are built:
	 * one for the missing packet(s) and one for the captured packet.
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
	 * @return the next record, or null if none
	 */
	const record* next();
};

} /* namespace horace */

#endif
