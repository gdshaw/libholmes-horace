// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EUI_ATTRIBUTE
#define LIBHOLMES_HORACE_EUI_ATTRIBUTE

#include <string>

#include "horace/attribute.h"

namespace horace {

/** A class to represent an EUI attribute. */
class eui_attribute:
	public attribute {
private:
	/** The EUI, in binary form. */
	std::string _eui;
public:
	/** Construct EUI attribute.
	 * @param eui the EUI, in binary form
	 */
	explicit eui_attribute(const std::string& eui);

	/** Construct EUI attribute from an octet reader.
	 * The type and length fields must already have been read. This
	 * constructor must read exactly the specified number of octets.
	 * @param in the octet reader
	 * @param length the length of the content, in octets
	 */
	eui_attribute(octet_reader& in, size_t length);

	/** Get the EUI.
	 * @return the EUI, in binary form
	 */
	const std::string& eui() const {
		return _eui;
	}

	virtual int type() const;
	virtual size_t length() const;
	virtual void write(std::ostream& out) const;
	virtual void write(octet_writer& out) const;
};

} /* namespace horace */

#endif
