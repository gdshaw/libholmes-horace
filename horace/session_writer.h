// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_WRITER
#define LIBHOLMES_HORACE_SESSION_WRITER

#include <string>

namespace horace {

class record;
class session_start_record;

/** A class for writing sessions to an endpoint. */
class session_writer {
private:
	/** The source ID. */
	std::string _source_id;

	/** The current start of session record, or null if none. */
	const session_start_record* _srec;
private:
	/** Process a start of session record.
	 * @param srec the start of session record
	 */
	void _process_session_start(const session_start_record& srec);
protected:
	/** Handle the start of a new session.
	 * @param srec the start of session record
	 */
	virtual void handle_session_start(const session_start_record& srec) = 0;

	/** Handle an event.
	 * @param rec the event record
	 */
	virtual void handle_event(const record& rec) = 0;
public:
	/** Construct session writer.
	 * @param source_id the required source ID
	 */
	session_writer(const std::string& source_id);

	virtual ~session_writer() = default;

	/** Get the source ID. */
	const std::string& source_id() const {
		return _source_id;
	}

	/** Get the current session start record.
	 * If there is no active start record then an exception is thrown.
	 * @return the start record
	 */
	const session_start_record& start_record() const;

	/** Write a record to the endpoint. */
	void write(const record& rec);
};

} /* namespace horace */

#endif
