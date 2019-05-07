// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/record.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"
#include "horace/session_writer.h"

namespace horace {

session_writer::session_writer(const std::string& source_id):
	_source_id(source_id),
	_srec(0) {}

void session_writer::_process_session_start(const session_start_record& srec) {
	bool match = _srec && _srec->matches(srec);
	if (!match) {
		_srec = &srec;
		handle_session_start(srec);
	}
}

void session_writer::_process_session_end(const session_end_record& erec) {
	handle_session_end(erec);
	_srec = 0;
}

const session_start_record& session_writer::start_record() const {
	if (!_srec) {
		throw horace_error("no session in progress");
	}
	return *_srec;
}

void session_writer::write(const record& rec) {
	switch(rec.type()) {
	case record::REC_SESSION_START:
		_process_session_start(dynamic_cast<const session_start_record&>(rec));
		break;
	case record::REC_SESSION_END:
		_process_session_end(dynamic_cast<const session_end_record&>(rec));
		break;
	default:
		if (rec.type() >= record::REC_EVENT_MIN) {
			handle_event(rec);
		}
	}
}

} /* namespace horace */
