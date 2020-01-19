// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ATTRIBUTE
#define LIBHOLMES_HORACE_ATTRIBUTE

#include <memory>
#include <iosfwd>

namespace horace {

class octet_reader;
class octet_writer;
class session_context;

// Attribute format codes.
static const int attrfmt_compound = 0;
static const int attrfmt_unsigned_integer = 1;
static const int attrfmt_signed_integer = 2;
static const int attrfmt_binary = 3;
static const int attrfmt_string = 4;
static const int attrfmt_timestamp = 5;

// Reserved attribute IDs
static const int attrid_attr_def = -1;
static const int attrid_attr_id = -2;
static const int attrid_attr_name = -3;
static const int attrid_attr_fmt = -4;
static const int attrid_channel_def = -5;
static const int attrid_channel_num = -6;
static const int attrid_channel_label = -7;
static const int attrid_source = -8;
static const int attrid_seqnum = -9;
static const int attrid_ts_begin = -10;
static const int attrid_ts_end = -11;
static const int attrid_hash = -12;
static const int attrid_hash_alg = -13;
static const int attrid_sig = -14;
static const int attrid_sig_alg = -15;
static const int attrid_sig_pubkey = -16;
static const int attrid_time_system = -17;

/** An abstract base class to represent a HORACE attribute. */
class attribute {
private:
	/** The attribute ID. */
	int _attrid;
public:
	/** Construct attribute.
	 * @param id the required attribute ID
	 */
	explicit attribute(int attrid):
		_attrid(attrid) {}

	virtual ~attribute() = default;

	/** Test whether two attributes are equal.
	 * Attributes compare equal iff they have the same attribute ID
	 * and equivalent content. In most cases this will imply that they
	 * also have the same dynamic type, however exceptions are possible
	 * such as binary_attribute and binary_ref_attribute.
	 * @param that the attribute to be compared with this
	 * @return true if equal, otherwise false
	 */
	virtual bool operator==(const attribute& that) const = 0;

	/** Test whether two attributes are not equal.
	 * @param that the attribute to be compared with this
	 * @return true if not equal, otherwise false
	 */
	bool operator!=(const attribute& that) const {
		return !(*this == that);
	}

	/** Get the attribute ID
	 * @return the attribute ID
	 */
	int attrid() const {
		return _attrid;
	}

	/** Get the length of the content of this attribute.
	 * @return the content length, in octets
	 */
	virtual size_t length() const = 0;

	/** Create a deep copy of this attribute.
	 * @return the cloned copy
	 */
	virtual std::unique_ptr<attribute> clone() const = 0;

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
	 * The ID and length fields must already have been read. This
	 * function must read exactly the specified number of octets.
	 * @param session the applicable session context
	 * @param attrid the attribute ID
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 * @return the resulting attribute
	 */
	static std::unique_ptr<attribute> parse(session_context& session,
		int attrid, size_t length, octet_reader& in);

	/** Parse attribute from an octet reader.
	 * The ID and length fields must not already have been read.
	 * @param session the applicable session context
	 * @param in the octet reader
	 * @return the resulting attribute
	 */
	static std::unique_ptr<attribute> parse(session_context& session,
		octet_reader& in);

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
