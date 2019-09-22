// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/packet_record_builder.h"

namespace horace {

packet_record_builder::packet_record_builder():
	_ts_attr(ATTR_TIMESTAMP, 0, 0),
	_pkt_attr(ATTR_PACKET, 0, 0),
	_origlen_attr(ATTR_PACKET_LENGTH, 0),
	_rpt_attr(ATTR_REPEAT, 0),
	_count(0) {

	_buffer.emplace_back(0 + record::channel_packet);
	_buffer.emplace_back(0 + record::channel_packet);
}

void packet_record_builder::build_packet(const struct timespec* ts,
	const void* content, size_t origlen, size_t snaplen,
	unsigned int dropped) {

	_count = 0;

	record_builder& pkt_builder = _buffer[_count];
	pkt_builder.reset();
	if (ts) {
		pkt_builder.append(_ts_attr = timestamp_attribute(ATTR_TIMESTAMP, *ts));
	}
	pkt_builder.append(_pkt_attr = binary_ref_attribute(ATTR_PACKET, snaplen, content));
	if (snaplen != origlen) {
		pkt_builder.append(_origlen_attr = unsigned_integer_attribute(ATTR_PACKET_LENGTH, origlen));
	}
	_count += 1;

	if (dropped != 0) {
		record_builder& drop_builder = _buffer[_count];
		drop_builder.reset();
		if (ts) {
			drop_builder.append(_ts_attr);
		}
		drop_builder.append(_rpt_attr = unsigned_integer_attribute(ATTR_REPEAT, dropped));
		_count += 1;
	}
}

void packet_record_builder::build_dropped(const struct timespec* ts,
	unsigned int dropped) {

	_count = 0;

	if (dropped != 0) {
		record_builder& drop_builder = _buffer[_count];
		drop_builder.reset();
		if (ts) {
			drop_builder.append(_ts_attr = timestamp_attribute(ATTR_TIMESTAMP, *ts));
		}
		drop_builder.append(_rpt_attr = unsigned_integer_attribute(ATTR_REPEAT, dropped));
		_count += 1;
	}
}

const record* packet_record_builder::next() {
	if (_count == 0) {
		return 0;
	}
	_count -= 1;
	return &_buffer[_count];
}

} /* namespace horace */
