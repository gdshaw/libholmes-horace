// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ATTRIBUTE_LIST
#define LIBHOLMES_HORACE_ATTRIBUTE_LIST

#include <vector>
#include <iostream>

#include "horace/octet_reader.h"
#include "horace/octet_writer.h"
#include "horace/attribute.h"

namespace horace {

/** A class to represent an ordered list of attributes. */
class attribute_list {
private:
	/** All attributes of this record. */
	std::vector<const attribute*> _attributes;

	/** The attribute owned by this record. */
	std::vector<const attribute*> _owned_attributes;
public:
	attribute_list() = default;
	virtual ~attribute_list();

	attribute_list(const attribute_list&) = delete;
	attribute_list(attribute_list&& that);

	attribute_list& operator=(const attribute_list&) = delete;
	attribute_list& operator=(attribute_list&&) = delete;

	/** Build attribute list from octet reader.
	 * It is presumed that the length field has already been read.
	 * This function must read exactly the specified number of octets.
         * @param in the octet reader
         * @param length the length of the content, in octets
	 */
	attribute_list(octet_reader& in, size_t length);

	/** Determine whether this attribute list is empty.
	 * @return true if empty, otherwise false
	 */
	bool empty() const {
		return _attributes.empty();
	}

	/** Get the length of the content of the attributes.
	 * @return the content length, in octets
	 */
	size_t length() const;

	/** Get the attribute list for this record.
	 * @return the attribute list
	 */
	const std::vector<const attribute*>& attributes() const {
		return _attributes;
	}
private:
	/** Find a single instance of a given attribute type.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param type the required attribute type
	 */
	const attribute& _find_one(int type) const;
public:
	/** Find a single instance of a given attribute type.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param type the required attribute type
	 */
	template<class T>
	const T& find_one(int type) const {
		return dynamic_cast<const T&>(_find_one(type));
	}

	/** Append an attribute to this list, with transfer of ownership.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	attribute_list& append(std::unique_ptr<attribute>& attr);

	/** Append an attribute to this list, with transfer of ownership.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	attribute_list& append(std::unique_ptr<attribute>&& attr);

	/** Append an attribute to this record, without transfer of ownership.
	 * It is the caller's responsibility to ensure that the attribute
	 * remains in existence until there is no further possibility of it
	 * being accessed.
	 * @param attr the attribute to be appended
	 * @return a reference to this
	 */
	attribute_list& append(const attribute& attr);

	/** Write this attribute list to an octet writer.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Write a record in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attr the record to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out,
		const attribute_list& attrlist);
};

} /* namespace horace */

#endif
