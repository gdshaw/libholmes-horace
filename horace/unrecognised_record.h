// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_UNRECOGNISED_RECORD
#define LIBHOLMES_HORACE_UNRECOGNISED_RECORD

#include "horace/record.h"

namespace horace {

/** A class to represent a record of arbitrary type. */
class unrecognised_record:
	public record {
public:
	/** Move-construct from generic record.
	 * @param rec the record
	 */
	unrecognised_record(record&& rec):
		record(rec) {}
};

} /* namespace horace */

#endif
