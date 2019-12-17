// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/retry_exception.h"
#include "horace/terminate_exception.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/hash.h"
#include "horace/keypair.h"
#include "horace/binary_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/attribute_list.h"
#include "horace/record.h"
#include "horace/session_builder.h"
#include "horace/endpoint_error.h"
#include "horace/endpoint.h"
#include "horace/session_writer_endpoint.h"
#include "horace/session_writer.h"
#include "horace/new_session_writer.h"

namespace horace {

new_session_writer::new_session_writer(endpoint& ep, session_builder& sb,
	const std::string& source_id, hash* hashfn,
	keypair* kp, unsigned long sigrate):
	_ep(dynamic_cast<session_writer_endpoint*>(&ep)),
	_source_id(source_id),
	_srec(0),
	_seqnum(0),
	_signature_ts({0}),
	_hashfn(hashfn),
	_kp(kp),
	_sigrate(sigrate) {

	if (hashfn) {
		sb.define_hash(*hashfn);
	}
	if (kp) {
		sb.define_keypair(*kp);
	}

	if (!_ep) {
		throw endpoint_error(
			"destination endpoint unable to receive sessions");
	}

	if (clock_gettime(CLOCK_REALTIME, &_signature_ts) == -1) {
		throw libc_error();
	}
}

bool new_session_writer::_signature_due() {
	timespec current_ts;
	if (clock_gettime(CLOCK_REALTIME, &current_ts) == -1) {
		throw libc_error();
	}

	timespec diff_ts;
	diff_ts.tv_sec = current_ts.tv_sec - _signature_ts.tv_sec;
	diff_ts.tv_nsec = current_ts.tv_nsec - _signature_ts.tv_nsec;
	if (diff_ts.tv_nsec < 0) {
		diff_ts.tv_sec -= 1;
		diff_ts.tv_nsec += 1000000000;
	}
	_signature_ts = current_ts;

	long diff = (diff_ts.tv_sec * 1000) + (diff_ts.tv_nsec / 1000000);
	return (diff >= _sigrate);
}

void new_session_writer::_write_signature(size_t hash_len, const void* hash) {
	std::string sig = _kp->sign(hash, hash_len);
	attribute_list sigattrs;
	sigattrs.append(std::make_unique<binary_attribute>(
		attrid_sig, sig.length(), sig.data()));
	record sigrec(channel_signature, std::move(sigattrs));
	try {
		_sw->write(sigrec);
	} catch (terminate_exception&) {
		throw;
	} catch (std::exception& ex) {
		_sw = 0;
		throw retry_exception();
	}
}

void new_session_writer::_open() {
	if (!_srec) {
		throw horace_error("session not yet started");
	}
	_sw = _ep->make_session_writer(_source_id);
	try {
		_sw->write(*_srec);
	} catch (terminate_exception&) {
		throw;
	} catch (std::exception& ex) {
		_sw = 0;
		throw retry_exception();
	}
	_srec->log(*log);
}

void new_session_writer::_write_event(const record& rec) {

	// Attempt to write record to destination.
	try {
		_sw->write(rec);
	} catch (terminate_exception&) {
		throw;
	} catch (std::exception& ex) {
		_sw = 0;
		throw retry_exception();
	}

	if (_hashfn) {
		rec.write(*_hashfn);
		const void* hash = _hashfn->final();
		size_t hash_len = _hashfn->length();

		_hattr = std::make_unique<binary_attribute>(attrid_hash,
			hash_len, hash);

		// The following method for regulating signature
		// generation is imperfect, because it checks the
		// clock only when an event is written (and may
		// therefore delay signing for an arbitrarily long
		// time). However, this will be easier to address
		// once support for multiple event sources has been
		// added.
		if (_kp && _signature_due()) {
			_write_signature(hash_len, hash);
		}
	}
}

void new_session_writer::begin_session(const record& srec) {
	std::scoped_lock lk(_mutex);

	_srec = &srec;
	try {
		_open();
	} catch (retry_exception&) {
		if (log->enabled(logger::log_err)) {
			log_message msg(*log, logger::log_err);
			msg << "error during capture (will retry)";
		}
	}
}

void new_session_writer::write_event(const record& rec) {
	std::scoped_lock lk(_mutex);

	// Append sequence number to record.
	// This is the same method as was previously used in
	// spoolfile_writer, and is known to be inefficient.
	// It was tolerable in that context because it was
	// invoked only once per spoolfile. Here it is
	// applied to every event record, and effectively
	// undoes efforts made elsewhere to avoid copying.
	// A more efficient solution would be desirable, but
	// the current event reader API does not allow for one.
	attribute_list attrs = rec.attributes();
	unsigned_integer_attribute seqnum_attr(attrid_seqnum, _seqnum++);
	attrs.append(seqnum_attr);
	if (_hattr) {
		attrs.append(std::move(_hattr));
	}
	record nrec(rec.channel_number(), std::move(attrs));

	bool retry = true;
	while (retry) {
		retry = false;

		try {
			if (!_sw) {
				_open();
			}
			_write_event(nrec);
		} catch (retry_exception&) {
			retry = true;
			if (log->enabled(logger::log_err)) {
				log_message msg(*log, logger::log_err);
				msg << "error during capture (will retry)";
			}
		} catch (std::exception& ex) {
			if (log->enabled(logger::log_err)) {
				log_message msg(*log, logger::log_err);
				msg << ex.what();
			}
		}
	}
}

void new_session_writer::end_session() {
	std::scoped_lock lk(_mutex);

	if (_sw) {
		attribute_list attrs;
		attrs.append(_srec->find_one<string_attribute>(attrid_source).clone());
		attrs.append(_srec->find_one<timestamp_attribute>(attrid_ts_begin).clone());
		attrs.append(std::make_unique<timestamp_attribute>(attrid_ts_end));
		std::unique_ptr<record> erec = std::make_unique<record>(channel_session, attrs);
		_sw->write(*erec);
		erec->log(*log);
		_sw = 0;
	}
}

} /* namespace horace */
