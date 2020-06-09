// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <iostream>

#include "poll.h"

#include "horace/terminate_exception.h"
#include "horace/libc_error.h"
#include "horace/terminate_flag.h"
#include "horace/keypair.h"
#include "horace/binary_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/attribute_list.h"
#include "horace/record.h"
#include "horace/event_signer.h"
#include "horace/session_builder.h"
#include "horace/new_session_writer.h"

namespace horace {

void event_signer::_run() {
	// Create a lock for locking the mutex.
	// Note that holding this lock will, in short order, prevent events
	// from being captured by any event source. For this reason it
	// should not be held longer than necessary. In particular:
	// - It should not be held during the delay period
	// - It should not be held while the signature is being generated
	std::unique_lock<std::mutex> lk(_mutex, std::defer_lock);

	try {
		while (true) {
			// Wait for an event to be available for signing.
			lk.lock();
			while (terminating.poll(), _hash.empty()) {
				_cv.wait(lk);
			}
			lk.unlock();

			// Sleep for the requested delay period before signing.
			terminating.millisleep(_delay);

			// Get the sequence number and hash for the most recent
			// event record captured. Note that this may be different
			// from the event record which triggered the condition
			// variable.
			// Following this operation, _hash is left empty to
			// indicate that no further signatures should be
			// generated until a new event record is seen.
			lk.lock();
			uint64_t sig_seqnum = _seqnum;
			std::basic_string<unsigned char> sig_hash;
			sig_hash.swap(_hash);
			lk.unlock();

			// Sign the hashed record, then record the signature.
			auto sigrec = make_signature(&sig_seqnum, sig_hash);
			_nsw->write_signature(*sigrec);
		}
	} catch (terminate_exception&) {
		// No action.
	} catch (std::exception& ex) {
		std::cerr << ex.what() << std::endl;
	}
}

void event_signer::_do_run(event_signer& signer) {
	signer._run();
}

event_signer::event_signer(new_session_writer& nsw, keypair& kp,
	long delay):
	_nsw(&nsw),
	_kp(&kp),
	_delay(delay) {

	_thread = std::thread(_do_run, std::ref(*this));
}

void event_signer::build_session(session_builder& sb) {
	sb.define_keypair(*_kp);
}

std::unique_ptr<record> event_signer::make_signature(uint64_t* seqnum,
	const std::basic_string<unsigned char>& hash) {

	std::string sig = _kp->sign(hash.data(), hash.length());
	attribute_list sigattrs;
	if (seqnum) {
		sigattrs.insert(std::make_unique<unsigned_integer_attribute>(
			attrid_seqnum, *seqnum));
	}
	sigattrs.insert(std::make_unique<binary_attribute>(
		attrid_sig, sig.length(), sig.data()));
	return std::make_unique<record>(channel_signature,
		std::move(sigattrs));
}

void event_signer::handle_event(uint64_t seqnum,
	const std::basic_string<unsigned char>& hash) {

	{
		std::lock_guard<std::mutex> lk(_mutex);
		_seqnum = seqnum;
		_hash = hash;
	}
	_cv.notify_one();
}

void event_signer::stop() {
	// The thread may be waiting on the condition variable,
	// so send it a notification. The mutex must be locked
	// and then unlocked first, to avoid a potential race
	// condition.
	std::unique_lock<std::mutex> lk(_mutex);
	lk.unlock();
	_cv.notify_one();

	// The thread should now exit gracefully.
	_thread.join();
}

} /* namespace horace */
