// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD
#define LIBHOLMES_HORACE_RECORD

#include <memory>
#include <list>
#include <iostream>

#include "horace/octet_writer.h"
#include "horace/attribute.h"

namespace horace {

class record_builder;

/** An abstract base class to represent a HORACE record. */
class record {
	friend record_builder;
private:
	/** The type of this record. */
	int _type;

	/** The attributes of this record. */
	std::list<std::shared_ptr<attribute>> _attributes;
protected:
	/** Construct record with empty attribute list. */
	explicit record(int type):
		_type(type) {}
public:
	// Control record type codes.
	static const int REC_SESSION_START = 0x00;
	static const int REC_SESSION_END = 0x01;

	// Event record type codes.
	static const int REC_EVENT_MIN = 0x20;
	static const int REC_PACKET = 0x20;

	virtual ~record() = default;

	/** Get the type of this record.
	 * @return type the type code
	 */
	virtual int type() const {
		return _type;
	}

	/** Get the type name of this record.
	 * @return type the type name
	 */
	virtual std::string type_name() const;

	/** Get the length of the content of this record.
	 * @return the content length, in octets
	 */
	size_t length() const;

	/** Get the attribute list for this record.
	 * @return the attribute list
	 */
	const std::list<std::shared_ptr<attribute>> attributes() const {
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
