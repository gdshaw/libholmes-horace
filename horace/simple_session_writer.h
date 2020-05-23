// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SIMPLE_SESSION_WRITER
#define LIBHOLMES_HORACE_SIMPLE_SESSION_WRITER

#include "horace/session_writer.h"

namespace horace {

/** A partial implementation of session_writer providing commonly-needed
 * functionality. */
class simple_session_writer:
	public session_writer {
private:
	/** The current session record, or null if none. */
	std::unique_ptr<const record> _srec;

	/** The current sequence number, or 0 if unspecified. */
	uint64_t _seqnum;

	/** True if the current session has ended, or if there is
	 * no current session, otherwise false. */
	bool _ended;

	/** A record available for reading, or null if none. */
	std::unique_ptr<record> _reply;
private:
	/** Process a session record.
	 * @param srec the session record
	 */
	void _process_session_record(const record& srec);

	/** Process an end of data record.
	 * @param erec the end record
	 */
	void _process_end_record(const record& erec);

	/** Process a sync record.
	 * @param crec the sync record
	 */
	void _process_sync_record(const record& crec);
protected:
	/** Handle the start of a new session.
	 * @param srec the session record
	 */
	virtual void handle_session_start(const record& srec) = 0;

	/** Handle the end of a session.
	 * @param erec the end record
	 */
	virtual void handle_session_end(const record& erec) = 0;

	/** Handle synchronisation request.
	 * @param crec the synchronisation record
	 */
	virtual void handle_sync(const record& crec) = 0;

	/** Handle signature.
	 * @param grec the signature record
	 */
	virtual void handle_signature(const record& grec);

	/** Handle an event.
	 * @param rec the event record
	 */
	virtual void handle_event(const record& rec) = 0;
protected:
	/** Get the current session record.
	 * If there is no session in progress then an exception is thrown.
	 * @return the current session record
	 */
	const record& session_record() const;
public:
	/** Construct simple session writer.
	 * @param srcid the required source ID
	 */
	explicit simple_session_writer(const std::string& srcid);

	virtual bool writable();
	virtual void write(const record& rec);
	virtual bool readable();
	virtual std::unique_ptr<record> read();
};

} /* namespace horace */

#endif
