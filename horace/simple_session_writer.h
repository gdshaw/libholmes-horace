// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SIMPLE_SESSION_WRITER
#define LIBHOLMES_HORACE_SIMPLE_SESSION_WRITER

#include "horace/session_writer.h"

namespace horace {

class session_start_record;
class session_end_record;
class sync_record;

/** A partial implementation of session_writer providing commonly-needed
 * functionality. */
class simple_session_writer:
	public session_writer {
private:
	/** The current start of session record, or null if none. */
	const session_start_record* _srec;

	/** A record available for reading, or null if none. */
	std::unique_ptr<record> _reply;
private:
	/** Process a start of session record.
	 * @param srec the start of session record
	 */
	void _process_session_start(const session_start_record& srec);

	/** Process an end of session record.
	 * @param erec the end of session record
	 */
	void _process_session_end(const session_end_record& erec);

	/** Process a sync record.
	 * @param crec the sync record
	 */
	void _process_sync(const sync_record& crec);
protected:
	/** Handle the start of a new session.
	 * @param srec the start of session record
	 */
	virtual void handle_session_start(const session_start_record& srec) = 0;

	/** Handle the end of a session.
	 * @param erec the end of session record
	 */
	virtual void handle_session_end(const session_end_record& erec) = 0;

	/** Handle synchronisation request.
	 * @param crec the synchronisation record
	 */
	virtual void handle_sync(const sync_record& crec) = 0;

	/** Handle an event.
	 * @param rec the event record
	 */
	virtual void handle_event(const record& rec) = 0;
protected:
	/** Get the current session start record.
	 * If there is no active start record then an exception is thrown.
	 * @return the session start record
	 */
	const session_start_record& start_record() const;
public:
	/** Construct simple session writer.
	 * @param source_id the required source ID
	 */
	simple_session_writer(const std::string& source_id);

	virtual void write(const record& rec);
	virtual std::unique_ptr<record> read();
};

} /* namespace horace */

#endif
