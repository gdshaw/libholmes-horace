// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_MONGODB_SESSION_WRITER
#define LIBHOLMES_HORACE_MONGODB_SESSION_WRITER

#include <memory>

#include "horace/simple_session_writer.h"

#include "mongodb_database.h"
#include "mongodb_collection.h"

namespace horace {

class packet_record;
class mongodb_endpoint;

/** A class for writing sessions to a MongoDB database. */
class mongodb_session_writer:
	public simple_session_writer {
private:
	/** The MongoDB database. */
	mongodb_database _database;

	/** The MongoDB collection in which sessions are recorded. */
	mongodb_collection _sessions;

	/** The MongoDB collection in which packets are recorded. */
	mongodb_collection _packets;

	/** The current session timestamp. */
	uint64_t _ts64;

	/** The current sequence number. */
	uint64_t _seqnum;

	/** The write concern (majority, journalled). */
	mongoc_write_concern_t* _wc;

	/** The options document for writing sessions. */
	bson_t _opts_session;

	/** The options document for creating a bulk operation. */
	bson_t _opts_bulk;

	/** The options document for bulk writing packets. */
	bson_t _opts_packet;

	/** The current bulk operation, or 0 if none. */
	mongoc_bulk_operation_t* _bulk;

	/** The number of outstanding bulk insertions. */
	unsigned int _bulk_count;

	/** Ensure details recorded for start of session. */
	void _start_session();

	/** Ensure all preceding data is has been record durably. */
	void _sync();

	/** Write packet document as part of bulk operation.
	 * @param doc the packet document to be written
	 */
	void _write_bulk(const bson_t& doc);
protected:
	/** Handle packet.
	 * @param prec the packet record to be handled
	 */
	virtual void handle_packet(const packet_record& prec);

	virtual void handle_session_start(const session_start_record& srec);
	virtual void handle_session_end(const session_end_record& erec);
	virtual void handle_sync(const sync_record& crec);
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
