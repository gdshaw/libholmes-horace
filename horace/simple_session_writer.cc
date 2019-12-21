// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"
#include "horace/simple_session_writer.h"

namespace horace {

simple_session_writer::simple_session_writer(const std::string& source_id):
	session_writer(source_id) {}

void simple_session_writer::_process_session_record(const record& srec) {
	// Check that the source ID is valid for this session writer.
	// If not, the session record must be rejected.
	std::string new_source_id =
		srec.find_one<string_attribute>(attrid_source).content();
	if (new_source_id != source_id()) {
		throw horace_error("unexpected source ID in session record");
	}

	if (!srec.contains(attrid_ts_end)) {
		// This is a start of session record.
		if (!_srec || !(
			_srec->find_one<timestamp_attribute>(attrid_ts_begin) ==
			srec.find_one<timestamp_attribute>(attrid_ts_begin))) {

			// The timestamp has changed: this record marks the
			// start of a new session.
			_srec = std::make_unique<record>(srec);
			handle_session_start(srec);
		} else if (*_srec != srec) {
			// The timestamp is unchanged, but the record does
			// not match for some other reason. This is an
			// unrecoverable error.
			throw horace_error("non-matching start of session record");
		}
	} else {
		// This is an end of session record.
		handle_session_end(srec);
		_srec = 0;
	}
}

void simple_session_writer::_process_sync_record(const record& crec) {
	handle_sync(crec);
	_reply = std::make_unique<record>(channel_sync, crec.attributes());
}

void simple_session_writer::handle_signature(const record& grec) {}

const record& simple_session_writer::session_record() const {
	if (!_srec) {
		throw horace_error("no session in progress");
	}
	return *_srec;
}

void simple_session_writer::write(const record& rec) {
	switch(rec.channel_number()) {
	case channel_session:
		_process_session_record(rec);
		break;
	case channel_sync:
		_process_sync_record(rec);
		break;
	case channel_signature:
		handle_signature(rec);
		break;
	default:
		if (rec.is_event()) {
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
