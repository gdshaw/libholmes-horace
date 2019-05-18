// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/record.h"
#include "horace/record_builder.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"
#include "horace/sync_record.h"
#include "horace/simple_session_writer.h"

namespace horace {

simple_session_writer::simple_session_writer(const std::string& source_id):
	session_writer(source_id),
	_srec(0) {}

void simple_session_writer::_process_session_start(const session_start_record& srec) {
	bool match = _srec && _srec->matches(srec);
	if (!match) {
		_srec = &srec;
		handle_session_start(srec);
	}
}

void simple_session_writer::_process_session_end(const session_end_record& erec) {
	handle_session_end(erec);
	_srec = 0;
}

void simple_session_writer::_process_sync(const sync_record& crec) {
	handle_sync(crec);
	record_builder builder(record::REC_ACK);
	for (std::shared_ptr<attribute> attr : crec.attributes()) {
		builder.append(attr);
	}
	_reply = builder.build();
}

const session_start_record& simple_session_writer::start_record() const {
	if (!_srec) {
		throw horace_error("no session in progress");
	}
	return *_srec;
}

void simple_session_writer::write(const record& rec) {
	switch(rec.type()) {
	case record::REC_SESSION_START:
		_process_session_start(dynamic_cast<const session_start_record&>(rec));
		break;
	case record::REC_SESSION_END:
		_process_session_end(dynamic_cast<const session_end_record&>(rec));
		break;
	case record::REC_SYNC:
		_process_sync(dynamic_cast<const sync_record&>(rec));
		break;
	default:
		if (rec.type() >= record::REC_EVENT_MIN) {
			handle_event(rec);
		}
	}
}

std::unique_ptr<record> simple_session_writer::read() {
	if (!_reply) {
		throw horace_error("no reply record available");
	}
	return std::move(_reply);
}

} /* namespace horace */