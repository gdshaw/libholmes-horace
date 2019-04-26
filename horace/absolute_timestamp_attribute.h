// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ABSOLUTE_TIMESTAMP_ATTRIBUTE
#define LIBHOLMES_HORACE_ABSOLUTE_TIMESTAMP_ATTRIBUTE

#include "horace/attribute.h"

namespace horace {

/** An abstract class to represent an absolute timestamp attribute.
 * Timestamp are classified as absolute or relative, depending on whether
 * they refer to a fixed epoch or are relative to another timestamp.
 * This distinction is important in start of session records, where
 * absolute timestamps may be used but relative timestamps may not.
 */
class absolute_timestamp_attribute:
	public attribute {};

} /* namespace horace */

#endif
