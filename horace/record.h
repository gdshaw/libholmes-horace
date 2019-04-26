// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD
#define LIBHOLMES_HORACE_RECORD

#include <memory>
#include <list>
#include <iostream>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/attribute.h"

namespace horace {

/** An abstract base class to represent a HORACE record. */
class record {
private:
	/** The attributes of this record. */
	std::list<std::shared_ptr<attribute>> _attributes;
protected:
	/** Construct record with empty attribute list. */
	record() = default;

	/** Construct record from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	record(octet_reader& in, size_t length);

	/** Append an attribute to this record.
	 * @param attr the attribute to be appended
	 */
	void append(std::shared_ptr<attribute> attr);
public:
	static const int REC_SESSION_START = 0x00;
	static const int REC_SESSION_END = 0x01;

	virtual ~record() = default;

	/** Get the type of this record.
	 * @return type the type code
	 */
	virtual int type() const = 0;

	/** Get the type name of this record.
	 * @return type the type name
	 */
	virtual std::string type_name() const = 0;

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

	/** Write this record in human-readable form to an output stream.
	 * This functionality can also be accessed via operator<<,
	 * however a virtual member function is needed so that the
	 * behaviour can vary polymorphically.
	 * @param out the stream to which the output should be written
	 */
	void write(std::ostream& out) const;

	/** Write this record to an octet writer.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Parse record from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param type the record type
	 * @param length the length of the content, in octets
	 * @return the resulting record
	 */
	static std::unique_ptr<record> parse(octet_reader& in,
		int type, size_t length);

	/** Parse record from an octet reader.
	 * It is presumed that the type and length fields have not already
	 * been read.
	 * @param in the octet reader
	 * @return the resulting record
	 */
	static std::unique_ptr<record> parse(octet_reader& in);

	/** Write a record in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attr the record to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out,
		const record& rec) {

		rec.write(out);
		return out;
        }
};

} /* namespace horace */

#endif
