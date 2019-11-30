// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD
#define LIBHOLMES_HORACE_RECORD

#include <iosfwd>

#include "horace/attribute_list.h"

namespace horace {

class logger;
class octet_reader;
class octet_writer;
class attribute;
class session_context;

// Reserved channel numbers.
static const int channel_error = -1;
static const int channel_session = -2;
static const int channel_sync = -3;

/** An abstract base class to represent a HORACE record. */
class record {
private:
	/** The channel number. */
	int _channel;

	/** The attribute list. */
	attribute_list _attributes;
public:
	/** Construct empty record.
	 * This is provided so that a record can be a member of a
	 * standard container.
	 */
	record():
		_channel(0) {}

	/** Construct record from attribute list.
	 * @param channel the channel number
	 * @param attributes the list of attributes
	 */
	record(int channel, const attribute_list& attributes):
		_channel(channel),
		_attributes(attributes) {}

	/** Construct record from attribute list.
	 * @param channel the channel number
	 * @param attributes the list of attributes
	 */
	record(int channel, attribute_list&& attributes):
		_channel(channel),
		_attributes(std::move(attributes)) {}

	/** Construct record from octet reader.
	 * The channel and length fields must not already have been read.
	 * @param session the applicable session information object
	 * @param in the octet reader
	 */
	record(session_context& session, octet_reader& in);

	/** Get the channel number for this record.
	 * @return the channel number
	 */
	virtual int channel_number() const {
		return _channel;
	}

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

	/** Get the attribute list for this record.
	 * @return the attribute list
	 */
	const attribute_list& attributes() const {
		return _attributes;
	}

	/** Determine whether this record contains any instances of a given
	 * attribute type.
	 * @param type the required attribute type
	 * @return true if there are one or more instances, otherwise false
	 */
	bool contains(int type) const {
		return _attributes.contains(type);
	}

	/** Find a single instance of a given attribute type.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param type the required attribute type
	 */
	template<class T>
	const T& find_one(int type) const {
		return _attributes.find_one<T>(type);
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

bool same_session(const record& lhs, const record& rhs);

} /* namespace horace */

#endif
