// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NEW_SESSION_WRITER
#define LIBHOLMES_HORACE_NEW_SESSION_WRITER

#include <memory>
#include <cstdint>
#include <ctime>
#include <mutex>

#include "horace/source_id.h"
#include "horace/binary_attribute.h"
#include "horace/session_writer.h"

namespace horace {

class hash;
class keypair;
class record;
class session_builder;
class endpoint;
class session_writer_endpoint;
class event_signer;

/** A class for writing newly-captured sessions to an endpoint. */
class new_session_writer {
private:
	/** A mutex to allow thread-safe capture from multiple sources. */
	std::mutex _mutex;

	/** The destination endpoint, as a session_writer_endpoint. */
	session_writer_endpoint* _ep;

	/** The required source ID. */
	source_id _srcid;

	/** The session record, or 0 if not yet created. */
	const record* _srec;

	/** A session writer for the destination endpoint. */
	std::unique_ptr<session_writer> _sw;

	/** The sequence number for the next event record. */
	uint64_t _seqnum;

	/** A hash attribute for the most recent event record,
	 * or 0 if none. */
	std::unique_ptr<binary_attribute> _hattr;

	/** The hash function to apply to each event record,
	 * or 0 for none. */
	hash* _hashfn;

	/** An event signer for signing event records, or 0 if none. */
	event_signer* _signer;

	/** Write any type of record to the endpoint (with retry).
	 * @param rec the record to be written
	 */
	void _write(const record& rec);
public:
	/** Construct new session writer.
	 * @param ep the destination endpoint
	 * @param sb a session builder for the new session
	 * @param srcid the required source ID
	 * @param hashfn the hash function to apply to each event record,
	 *  or 0 if none
	 */
	new_session_writer(endpoint& ep, session_builder& sb,
		const std::string& srcid, hash* hashfn);

	/** Test whether the endpoint is writable.
	 * This function has the same behaviour as
	 * horace::session_writer::writable.
	 * @return true if writable, otherwise false
	 */
	bool writable();

	/** Attach an event signer.
	 * @param signer the event signer to be attached
	 */
	void attach_signer(event_signer& signer);

	/** Begin the session.
	 * @param srec the required session record
	 */
	void begin_session(const record& srec);

	/** Write an event record to the endpoint (with retry).
	 * @param rec the event record to be written
	 */
	void write_event(const record& rec);

	/** Write an signature record to the endpoint (with retry).
	 * @param sigrec the event record to be written
	 */
	void write_signature(const record& sigrec);

	/** End the session. */
	void end_session();
};

} /* namespace horace */

#endif
