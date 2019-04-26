// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNRECOGNISED_RECORD
#define LIBHOLMES_HORACE_UNRECOGNISED_RECORD

#include <memory>
#include <list>
#include <iostream>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/attribute.h"
#include "horace/record.h"

namespace horace {

/** A class to represent a record of arbitrary type. */
class unrecognised_record:
	public record {
private:
	/** The type of this record. */
	int _type;

	/** The attributes of this record. */
	std::list<std::shared_ptr<attribute>> _attributes;
public:
	/** Construct unrecognised record from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param type the record type
	 * @param length the length of the content, in octets
	 */
	unrecognised_record(octet_reader& in, int type, size_t length);

	virtual int type() const {
		return _type;
	}

	virtual std::string type_name() const;
};

} /* namespace horace */

#endif
