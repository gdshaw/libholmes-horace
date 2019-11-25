// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_RECORD_BUILDER
#define LIBHOLMES_HORACE_RECORD_BUILDER

#include <memory>

#include "horace/record.h"

namespace horace {

class octet_reader;
class attribute;
class session_context;

/** A class for incrementally building HORACE records. */
class record_builder:
	public record {
public:
	/** Construct record builder with empty attribute list.
	 * @param channel the channel number of the record to be built
	 */
	explicit record_builder(int channel);

	/** Build record from octet reader.
	 * It is presumed that the channel and length fields have not already
	 * been read.
	 * @param session the applicable session information object
	 * @param in the octet reader
	 */
	record_builder(session_context& session, octet_reader& in);

	/** Append an attribute to this record, with transfer of ownership.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	record_builder& append(std::unique_ptr<attribute>& attr);

	/** Append an attribute to this record, with transfer of ownership.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	record_builder& append(std::unique_ptr<attribute>&& attr);

	/** Append an attribute to this record, without transfer of ownership.
	 * It is the caller's responsibility to ensure that the attribute
	 * remains in existence until there is no further possibility of it
	 * being accessed.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	record_builder& append(const attribute& attr);

	/** Remove all attributes from this record. */
	void reset();

	/** Build record.
	 * The record returned will be of a type appropriate to the
	 * specified channel number.
	 */
	std::unique_ptr<record> build();
};

} /* namespace horace */

#endif
