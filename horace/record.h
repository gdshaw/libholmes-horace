// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD
#define LIBHOLMES_HORACE_RECORD

#include <vector>
#include <iostream>

#include "horace/octet_writer.h"
#include "horace/attribute.h"

namespace horace {

class logger;
class record_builder;

/** An abstract base class to represent a HORACE record. */
class record {
	friend record_builder;
private:
	/** The channel number. */
	int _channel;

	/** All attributes of this record. */
	std::vector<const attribute*> _attributes;

	/** The attribute owned by this record. */
	std::vector<const attribute*> _owned_attributes;
protected:
	/** Construct record with empty attribute list.
	 * @param channel the channel number
	 */
	explicit record(int channel):
		_channel(channel) {}
public:
	// Reserved channel numbers.
	static const int channel_error = -1;
	static const int channel_session = -2;
	static const int channel_sync = -3;

	// Reserved channel numbers (transitional period only).
	static const int channel_packet = 0;
	static const int channel_session_end = -4;
	static const int channel_ack = -5;

	virtual ~record();

	record(const record&) = delete;
	record(record&& that);

	record& operator=(const record&) = delete;
	record& operator=(record&&) = delete;

	/** Get the channel number for this record.
	 * @return the channel number
	 */
	virtual int channel_number() const {
		return _channel;
	}

	/** Get the channel name for this record.
	 * @return the channel name
	 */
	std::string channel_name() const;

	/** Determine whether this is an event record.
	 * Event records have a non-negative channel number.
	 * @return true if an event record, otherwise false
	 */
	bool is_event() const {
		return _channel >= 0;
	}

	/** Update sequence number.
	 * If the record is an event record containing a seqnum attribute
	 * then the value of that attribute is returned, otherwise the
	 * expected value is used.
	 * @param seqnum the expected sequence number
	 * @return the updated sequence number
	 */
	virtual uint64_t update_seqnum(uint64_t seqnum) const;

	/** Log this record.
	 * @param log the logger
	 */
	virtual void log(logger& log) const;

	/** Determine whether this record is empty.
	 * @return true if empty, otherwise false
	 */
	bool empty() const {
		return _attributes.empty();
	}

	/** Get the length of the content of this record.
	 * @return the content length, in octets
	 */
	size_t length() const;

	/** Get the attribute list for this record.
	 * @return the attribute list
	 */
	const std::vector<const attribute*> attributes() const {
		return _attributes;
	}

	/** Write this record to an octet writer.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Write a record in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attr the record to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, const record& rec);
};

} /* namespace horace */

#endif
