// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SOURCE_ID
#define LIBHOLMES_HORACE_SOURCE_ID

#include <string>

namespace horace {

/** A class to represent a source ID.
 * A source ID can be represented by a character string, however wrapping
 * it within this class additionally causes it to be validated.
 *
 * To be valid a source ID must be non-empty, but no more than 255
 * characters in length. It must further be drawn exclusively from the
 * following set of characters from the Basic Latin Unicode block:
 *
 * - uppercase letters
 * - lowercase letters
 * - decimal digits
 * - hyphen-minus
 * - full stop (but not as the first character)
 */
class source_id {
private:
	/** The source ID as a string. */
	std::string _id;
public:
	/** Construct source ID.
	 * @param id the source ID as a string
	 */
	explicit source_id(const std::string& id);

	operator const std::string&() const {
		return _id;
	}
};

} /* namespace horace */

#endif
