// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <signal.h>
#include <unistd.h>

#include "horace/libc_error.h"
#include "horace/horace_error.h"
#include "horace/terminate_exception.h"
#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/hash.h"
#include "horace/keypair.h"
#include "horace/compound_attribute.h"
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
#include "horace/event_signer.h"
#include "horace/new_session_writer.h"

namespace horace {

new_session_writer::new_session_writer(endpoint& ep, session_builder& sb,
	const std::string& srcid, hash* hashfn):
	_ep(dynamic_cast<session_writer_endpoint*>(&ep)),
	_srcid(srcid),
	_srec(0),
	_seqnum(0),
	_hashfn(hashfn),
	_signer(0) {

	if (hashfn) {
		sb.define_hash(*hashfn);
	}

	if (!_ep) {
		throw endpoint_error(
			"destination endpoint unable to receive sessions");
	}
	_sw = _ep->make_session_writer(_srcid);
}

void new_session_writer::_write(const record& rec) {
	bool retry = true;
	while (retry) {
		retry = false;

		try {
			_sw->write(rec);
		} catch (terminate_exception&) {
			throw;
		} catch (std::exception& ex) {
			_sw = 0;
			retry = true;
			if (log->enabled(logger::log_err)) {
				log_message msg1(*log, logger::log_err);
				msg1 << ex.what();
				log_message msg2(*log, logger::log_err);
				msg2 << "error during capture (will retry)";
			}
		}

		// Data capture should be suspended if the session writer
		// is not able to accept it, however there is no mechanism
		// for this yet so current behaviour is to terminate.
		if (!_sw->writable()) {
			kill(getpid(), SIGALRM);
		}
	}
}

bool new_session_writer::writable() {
	try {
		return _sw->writable();
	} catch (terminate_exception&) {
		throw;
	} catch (std::exception& ex) {
		if (log->enabled(logger::log_err)) {
			log_message msg1(*log, logger::log_err);
			msg1 << ex.what();
			log_message msg2(*log, logger::log_err);
			msg2 << "endpoint not ready to receive data";
		}
	}
	return false;
}

void new_session_writer::attach_signer(event_signer& signer) {
	_signer = &signer;
}

void new_session_writer::begin_session(const record& srec) {
	std::lock_guard<std::mutex> lk(_mutex);

	_srec = &srec;
	_sw->write(*_srec);
	_srec->log(*log);
}

void new_session_writer::write_event(const record& rec) {
	std::lock_guard<std::mutex> lk(_mutex);

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
	unsigned_integer_attribute seqnum_attr(attrid_seqnum, _seqnum);
	attrs.append(seqnum_attr);
	if (_hattr) {
		attrs.append(std::move(_hattr));
	}
	record nrec(rec.channel_number(), std::move(attrs));

	// Write the record (with retry).
	_write(nrec);

	// Hash and sign the record if appropriate.
	if (_hashfn) {
		nrec.write(*_hashfn);
		const void* hash = _hashfn->final();
		size_t hash_len = _hashfn->length();

		_hattr = std::make_unique<binary_attribute>(attrid_hash,
			hash_len, hash);

		if (_signer) {
			std::basic_string<unsigned char> hash_str(
				static_cast<const unsigned char*>(hash),
				hash_len);
			_signer->handle_event(_seqnum, hash_str);
		}
	}

	// Increment the sequence number.
	// (This must not be done earlier, since its value is
	// needed for signing.)
	_seqnum++;
}

void new_session_writer::write_signature(const record& sigrec) {
	std::lock_guard<std::mutex> lk(_mutex);

	// Write the signature record (with retry).
	_write(sigrec);
}

void new_session_writer::end_session() {
	std::lock_guard<std::mutex> lk(_mutex);
	if (_sw) {
                attribute_list end_attrs;
		end_attrs.append(std::make_unique<timestamp_attribute>(attrid_ts));
		end_attrs.append(std::make_unique<unsigned_integer_attribute>(attrid_seqnum, _seqnum));
		if (_hattr) {
			end_attrs.append(std::move(_hattr));
		}

		attribute_list attrs(_srec->attributes());
		attrs.append(std::make_unique<compound_attribute>(
			attrid_end, std::move(end_attrs)));
		std::unique_ptr<record> srec = std::make_unique<record>(
			channel_session, std::move(attrs));
		_sw->write(*srec);
		srec->log(*log);
		_sw = 0;
	}
}

} /* namespace horace */
