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
	/** The type of this record. */
	int _type;

	/** All attributes of this record. */
	std::vector<const attribute*> _attributes;

	/** The attribute owned by this record. */
	std::vector<const attribute*> _owned_attributes;
protected:
	/** Construct record with empty attribute list. */
	explicit record(int type):
		_type(type) {}
public:
	// Control record type codes.
	static const int REC_ERROR = -1;
	static const int REC_SESSION_START = -2;
	static const int REC_SESSION_END = -3;
	static const int REC_SYNC = -4;
	static const int REC_ACK = -5;

	// Event record type codes.
	static const int REC_EVENT_MIN = 0;
	static const int REC_PACKET = 0;

	virtual ~record();

	record(const record&) = delete;
	record(record&& that);

	record& operator=(const record&) = delete;
	record& operator=(record&&) = delete;

	/** Get the type of this record.
	 * @return type the type code
	 */
	virtual int type() const {
		return _type;
	}

	/** Get the type name of this record.
	 * @return type the type name
	 */
	std::string type_name() const;

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
