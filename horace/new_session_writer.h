// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NEW_SESSION_WRITER
#define LIBHOLMES_HORACE_NEW_SESSION_WRITER

#include <memory>
#include <cstdint>

#include "horace/binary_attribute.h"
#include "horace/session_writer.h"

namespace horace {

class hash;
class keypair;
class record;
class endpoint;
class session_writer_endpoint;

/** A class for writing newly-captured sessions to an endpoint. */
class new_session_writer {
private:
	/** The destination endpoint, as a session_writer_endpoint. */
	session_writer_endpoint* _ep;

	/** The required source ID. */
	std::string _source_id;

	/** The session record, or 0 if not yet created. */
	const record* _srec;

	/** A session writer for the destination endpoint. */
	std::unique_ptr<session_writer> _sw;

	/** The sequence number for the next event record. */
	uint64_t _seqnum;

	/** A hash attribute for the most recent event record,
	 * or 0 if none. */
	std::unique_ptr<binary_attribute> _hattr;

	/** The timestamp for the most recent signature, or the
	 * start of the session if none. */
	struct timespec _signature_ts;

	/** The hash function to apply to each event record,
	 * or 0 for none. */
	hash* _hashfn;

	/** The keypair with which to sign each event record,
	 * or 0 for none. */
	keypair* _kp;

	/** The minimum time in milliseconds between signatures. */
	unsigned long _sigrate;

	/** Write a signature record for the given hash data.
	 * @param hash_len the length of the hash
	 * @param hash the value of the hash
	 */
	void _write_signature(size_t hash_len, const void* hash);

	/** Determine whether an event record is due to be signed.
	 * This function checks only whether a sufficient time has
	 * elapsed since the last signature. It does not check
	 * whether a keypair or hash function have been specified
	 * (which are also preconditions for signing).
	 *
	 * The act of calling this function resets the clock in
	 * preparation for the following signature, therefore it
	 * should only be called once per event record.
	 * @return true if a signature is due, otherwise false
	 */
	bool _signature_due();

	/** Open a connection to the endpoint. */
	void _open();

	/** Write an event record to the endpoint (without retry).
	 * @param rec the event record to be written
	 */
	void _write_event(const record& rec);
public:
	/** Construct new session writer.
	 * @param ep the destination endpoint
	 * @param source_id the required source ID
	 * @param hashfn the hash function to apply to each event record,
	 *  or 0 if none
	 * @param kp the keypair with which to sign each event record,
	 *  or 0 if none
	 * @param sigrate the minimum time in milliseconds between
	 *  signatures
	 */
	new_session_writer(endpoint& ep, const std::string& source_id,
		hash* hashfn, keypair* kp, unsigned long sigrate);

	/** Begin the session.
	 * @param srec the required session record
	 */
	void begin_session(const record& srec);

	/** Write an event record to the endpoint (with retry).
	 * @param rec the event record to be written
	 */
	void write_event(const record& rec);

	/** End the session. */
	void end_session();
};

} /* namespace horace */

#endif
