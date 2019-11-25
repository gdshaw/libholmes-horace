// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_SESSION_WRITER
#define LIBHOLMES_HORACE_MONGODB_SESSION_WRITER

#include "horace/session_context.h"
#include "horace/simple_session_writer.h"

#include "mongodb_database.h"

namespace horace {

class attribute;
class mongodb_collection;
class mongodb_endpoint;

/** A class for writing sessions to a MongoDB database. */
class mongodb_session_writer:
	public simple_session_writer {
private:
	/** The MongoDB database. */
	mongodb_database _database;

	/** The MongoDB collection in which sessions are recorded. */
	mongodb_collection* _sessions;

	/** The current session timestamp. */
	struct timespec _session_ts;

	/** The current session context. */
	session_context _session;

	/** The current sequence number. */
	uint64_t _seqnum;

	/** The write concern (majority, journalled). */
	mongoc_write_concern_t* _wc;

	/** The options document for writing sessions. */
	bson_t _opts_session;

	/** The options document for creating a bulk operation. */
	bson_t _opts_bulk;

	/** The options document for bulk writing events. */
	bson_t _opts_event;

	/** The current bulk operation, or 0 if none. */
	mongoc_bulk_operation_t* _bulk;

	/** The number of outstanding bulk insertions. */
	unsigned int _bulk_count;

	/** The channel number for the current bulk operation. */
	int _bulk_channel;

	/** Append attribute to BSON document.
	 * @param bson the BSON document
	 * @param attr the attribute to be appended
	 */
	void _append_bson(bson_t& bson, const attribute& attr);

	/** Ensure details recorded for start of session. */
	void _start_session();

	/** Ensure all preceding data is has been record durably. */
	void _sync();

	/** Write event document as part of bulk operation.
	 * @param channel_number the channel number
	 * @param channel_name the channel name
	 * @param doc the event document to be written
	 */
	void _write_bulk(int channel_number, const std::string& channel_name,
		const bson_t& doc);
protected:
	virtual void handle_session_start(const record& srec);
	virtual void handle_session_end(const record& srec);
	virtual void handle_sync(const record& crec);
	virtual void handle_event(const record& rec);
public:
	/** Construct MongoDB session writer.
	 * @param dst_ep the destination endpoint
	 * @param source_id the required source ID
	 */
	mongodb_session_writer(const mongodb_endpoint& dst_ep,
		const std::string& source_id);

	/** Destroy MongoDB session writer. */
	~mongodb_session_writer();
};

} /* namespace horace */

#endif
