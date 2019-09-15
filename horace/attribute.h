// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ATTRIBUTE
#define LIBHOLMES_HORACE_ATTRIBUTE

#include <memory>
#include <iostream>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"

namespace horace {

/** An abstract base class to represent a HORACE attribute. */
class attribute {
public:
	// Reserved attribute type codes.
	static const int attr_type_def = -1;
	static const int attr_type_code = -2;
	static const int attr_type_label = -3;
	static const int attr_type_format = -4;
	static const int attr_channel_def = -5;
	static const int attr_channel_num = -6;
	static const int attr_channel_label = -7;

	// Reserved attribute type codes (transitional period only).
	static const int ATTR_SOURCE = 0x00;
	static const int ATTR_SEQNUM = 0x01;
	static const int ATTR_PACKET = 0x02;
	static const int ATTR_PACKET_LENGTH = 0x03;
	static const int ATTR_TIMESTAMP = 0x04;
	static const int ATTR_REPEAT = 0x0a;
	static const int ATTR_NETIF = 0x0b;
	static const int ATTR_IFINDEX = 0x0c;
	static const int ATTR_IFNAME = 0x0d;
	static const int ATTR_LINKTYPE = 0x0e;
	static const int ATTR_EUI = 0x0f;
private:
	/** The type code for this attribute. */
	int _type;
public:
	/** Construct attribute.
	 * @param type the required type code
	 */
	explicit attribute(int type):
		_type(type) {}

	virtual ~attribute() = default;

	/** Get the type of this attribute.
	 * @return the type code
	 */
	int type() const {
		return _type;
	}

	/** Get the name of this attribute.
	 * @return the name
	 */
	std::string name() const;

	/** Get the length of the content of this attribute.
	 * @return the content length, in octets
	 */
	virtual size_t length() const = 0;

	/** Write this attribute in human-readable form to an output stream.
	 * This functionality can also be accessed via operator<<,
	 * however a virtual member function is needed so that the
	 * behaviour can vary polymorphically.
	 * @param out the stream to which the output should be written
	 */
	virtual void write(std::ostream& out) const = 0;

	/** Write this attribute to an octet writer.
	 * @param out the octet writer
	 */
	virtual void write(octet_writer& out) const = 0;

	/** Parse attribute from an octet reader.
	 * It is presumed that the type and length fields have already been
	 * read. This function must read exactly the specified number of
	 * octets.
	 * @param in the octet reader
	 * @param type the attribute type
	 * @param length the length of the content, in octets
	 * @return the resulting attribute
	 */
	static std::unique_ptr<attribute> parse(octet_reader& in,
		int type, size_t length);

	/** Parse attribute from an octet reader.
	 * It is presumed that the type and length fields have not already
	 * been read.
	 * @param in the octet reader
	 * @return the resulting attribute
	 */
	static std::unique_ptr<attribute> parse(octet_reader& in);

	/** Write an attribute in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attr the attribute to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out,
		const attribute& attr) {

		attr.write(out);
		return out;
	}
};

} /* namespace horace */

#endif
