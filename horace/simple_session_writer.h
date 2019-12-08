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

	/** A record available for reading, or null if none. */
	std::unique_ptr<record> _reply;
private:
	/** Process a session record.
	 * @param srec the session record
	 */
	void _process_session_record(const record& srec);

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
	 * @param srec the session record
	 */
	virtual void handle_session_end(const record& srec) = 0;

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
	 * @param source_id the required source ID
	 */
	explicit simple_session_writer(const std::string& source_id);

	virtual void write(const record& rec);
	virtual std::unique_ptr<record> read();
};

} /* namespace horace */

#endif
