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

// The supported protocol version.
extern std::unique_ptr<unsigned int> protocol_version;

// Reserved channel numbers.
static const int channel_session = -1;
static const int channel_end = -2;
static const int channel_error = -3;
static const int channel_warning = -4;
static const int channel_sync = -5;
static const int channel_signature = -6;

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

	/** Determine whether this record contains any attributes with
	 * a given attribute ID.
	 * @param attrid the required attribute ID
	 * @return true if there are one or more instances, otherwise false
	 */
	bool contains(int attrid) const {
		return _attributes.contains(attrid);
	}

	/** Find a single attribute with a given attribute ID.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param addrid the required attribute ID
	 */
	template<class T>
	const T& find_one(int attrid) const {
		return _attributes.find_one<T>(attrid);
	}

	/** Write this record to an octet writer.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Test whether two records are equal.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if equal, otherwise false
	 */
	friend bool operator==(const record& lhs,
		const record& rhs);

	/** Test whether two records are not equal.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if not equal, otherwise false
	 */
	friend bool operator!=(const record& lhs,
		const record& rhs) {

		return !(lhs == rhs);
	}

	/** Test whether one record is a prefix of another.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if lhs is a prefix of rhs, otherwise false
	 */
	friend bool operator<=(const record& lhs,
		const record& rhs);

	/** Write a record in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attr the record to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out, const record& rec);
};

} /* namespace horace */

#endif
