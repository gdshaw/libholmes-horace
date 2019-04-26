// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_TIMESTAMP_ATTRIBUTE
#define LIBHOLMES_HORACE_TIMESTAMP_ATTRIBUTE

#include "horace/attribute.h"

namespace horace {

/** An abstract class to represent a timestamp attribute.
 * This class should be used where either an absolute or a relative
 * timestamp is acceptable.
 */
class timestamp_attribute:
	public attribute {};

} /* namespace horace */

#endif
