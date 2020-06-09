// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/record.h"
#include "horace/simple_session_writer.h"

namespace horace {

namespace {

bool same_ts(const record& lhs, const record& rhs) {
	return lhs.find_one<timestamp_attribute>(attrid_ts) ==
		rhs.find_one<timestamp_attribute>(attrid_ts);
}

} /* anonymous namespace */

simple_session_writer::simple_session_writer(const std::string& srcid):
	session_writer(srcid),
	_seqnum(0),
	_ended(true) {}

void simple_session_writer::_process_session_record(const record& srec) {
	// Check that the source ID is valid for this session writer.
	// If not, the session record must be rejected.
	std::string new_srcid =
		srec.find_one<string_attribute>(attrid_source).content();
	if (new_srcid != srcid()) {
		throw horace_error("unexpected source ID in session record");
	}

	if (!_srec || !same_ts(*_srec, srec)) {
		// The timestamp has changed: this record marks the
		// start of a new session.
		_srec = std::make_unique<record>(srec);
		_seqnum = 0;
		_ended = false;
		handle_session_start(*_srec);
	} else if (*_srec == srec) {
		// This is an exact copy of the previous observed
		// session record: no action required.
	} else {
		// The timestamp is unchanged, but the session record
		// differs from the one previously observed.
		// This is an unrecoverable error.
		throw horace_error("non-matching start of session record");
	}
}

void simple_session_writer::_process_end_record(const record& erec) {
	handle_session_end(erec);
	_ended = true;
}

void simple_session_writer::_process_sync_record(const record& crec) {
	handle_sync(crec);

	// Construct synchronisation status record for return to caller.
	attribute_list attrs;
	attrs.insert(_srec->find_one<string_attribute>(attrid_source).clone());
	attrs.insert(_srec->find_one<timestamp_attribute>(attrid_ts).clone());
	attrs.insert(std::make_unique<unsigned_integer_attribute>(
		attrid_seqnum, _seqnum));
	_reply = std::make_unique<record>(channel_sync, std::move(attrs));
}

void simple_session_writer::handle_signature(const record& grec) {}

const record& simple_session_writer::session_record() const {
	if (!_srec) {
		throw horace_error("no session in progress");
	}
	return *_srec;
}

bool simple_session_writer::writable() {
	return true;
}

void simple_session_writer::write(const record& rec) {
	int channel = rec.channel_id();
	if (!_srec) {
		if (rec.channel_id() != channel_session) {
			throw horace_error("no session in progress");
		}
	} else if (_ended) {
		if (rec.is_event()) {
			throw horace_error("session has ended");
		}
	}

	switch(rec.channel_id()) {
	case channel_session:
		_process_session_record(rec);
		break;
	case channel_end:
		_process_end_record(rec);
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
			_seqnum = rec.find_one<unsigned_integer_attribute>(
				attrid_seqnum).content() + 1;
		}
	}
}

bool simple_session_writer::readable() {
	return !!_reply;
}

std::unique_ptr<record> simple_session_writer::read() {
	if (!_reply) {
		throw horace_error("no reply record available");
	}
	return std::move(_reply);
}

} /* namespace horace */
