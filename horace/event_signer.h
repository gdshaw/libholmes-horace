// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EVENT_SIGNER
#define LIBHOLMES_HORACE_EVENT_SIGNER

#include <cstddef>
#include <cstdint>
#include <string>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace horace {

class hash;
class keypair;
class session_builder;
class new_session_writer;

/** A class for asynchronously signing events.
 * There are two reasons for generating signatures in a separate thread: to
 * avoid stalling the event capture thread(s) while a signature is being
 * computed, and to allow signature records to be written when indicated by
 * the timer even if the capture thread(s) are blocked at the time.
 */
class event_signer {
private:
	/** The new session writer for which signatures are required. */
	new_session_writer* _nsw;

	/** The keypair with which to sign each event record. */
	keypair* _kp;

	/** The delay in milliseconds from when a new event is available
	 * for signing to when it or a successor is signed. */
	long _delay;

	/** The signing thread. */
	std::thread _thread;

	/** A mutex for controlling access to _seqnum and _current_hash. */
	std::mutex _mutex;

	/** A condition variable for notifying new events. */
	std::condition_variable _cv;

	/** The sequence number to which _hash refers. */
	uint64_t _seqnum;

	/** The most recent hash presented for potential signing. */
	std::basic_string<unsigned char> _hash;

	/** The function to be executed by the signing thread. */
	void _run();

	/** Static wrapper function for _run.
	 * @signer the event_signer object
	 */
	static void _do_run(event_signer& signer);
public:
	/** Construct event signer.
	 * @param nsw the new session writer for which signatures are required
	 * @param kp the keypair with which to sign each event record
	 * @param delay the day in milliseconds before signing.
	 */
	event_signer(new_session_writer& nsw, keypair& kp, long delay);

	/** Add any required attributes to the session record.
	 * @param sb a builder for the session record
	 */
	void build_session(session_builder& sb);

	/** Handle event.
	 * This has the effect of offering the event for signing, but it is
	 * for the event signer to decide whether and when to sign it.
	 * @param seqnum the sequence number
	 * @param hash the hash of the event record
	 */
	void handle_event(uint64_t seqnum,
		const std::basic_string<unsigned char>& hash);

	/** Stop signing events. */
	void stop();
};

} /* namespace horace */

#endif
