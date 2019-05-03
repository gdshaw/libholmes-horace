// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD_BUILDER
#define LIBHOLMES_HORACE_RECORD_BUILDER

#include <memory>

#include "horace/attribute.h"
#include "horace/record.h"

namespace horace {

/** A class for incrementally building HORACE records. */
class record_builder:
	public record {
public:
	/** Construct record builder with empty attribute list.
	 * @param type the type of the record to be built.
	 */
	record_builder(int type);

	/** Build record from octet reader.
	 * It is presumed that the type and length fields have not already
	 * been read.
	 * @param in the octet reader
	 */
	record_builder(octet_reader& in);

	/** Append an attribute to this record.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	record_builder& append(std::shared_ptr<attribute> attr);

	/** Remove all attributes from this record. */
	void reset();

	/** Build record.
	 * The record returned will be of a type appropriate to the
	 * specified type code.
	 */
	std::unique_ptr<record> build();
};

} /* namespace horace */

#endif
