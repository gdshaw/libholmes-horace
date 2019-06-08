// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/packet_record_builder.h"

namespace horace {

packet_record_builder::packet_record_builder():
	_pkt_attr(0, 0),
	_origlen_attr(0),
	_rpt_attr(0),
	_count(0) {

	_buffer.emplace_back(0 + record::REC_PACKET);
	_buffer.emplace_back(0 + record::REC_PACKET);
}

void packet_record_builder::build_packet(const struct timespec* ts,
	const void* content, size_t origlen, size_t snaplen,
	unsigned int dropped) {

	_count = 0;

	record_builder& pkt_builder = _buffer[_count];
	pkt_builder.reset();
	if (ts) {
		pkt_builder.append(_ts_attr = posix_timespec_attribute(*ts));
	}
	pkt_builder.append(_pkt_attr = packet_ref_attribute(content, snaplen));
	if (snaplen != origlen) {
		pkt_builder.append(_origlen_attr = packet_length_attribute(origlen));
	}
	_count += 1;

	if (dropped != 0) {
		record_builder& drop_builder = _buffer[_count];
		drop_builder.reset();
		if (ts) {
			drop_builder.append(_ts_attr);
		}
		drop_builder.append(_rpt_attr = repeat_attribute(dropped));
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
			drop_builder.append(_ts_attr = posix_timespec_attribute(*ts));
		}
		drop_builder.append(_rpt_attr = repeat_attribute(dropped));
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
