// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ATTRIBUTE_LIST
#define LIBHOLMES_HORACE_ATTRIBUTE_LIST

#include <memory>
#include <vector>
#include <iosfwd>

namespace horace {

class octet_reader;
class octet_writer;
class attribute;
class session_context;

/** A class to represent an ordered list of attributes.
 * When an attribute is added to a list of this type, ownership may
 * optionally be transferred to the list on a case-by-case basis.
 * This may be helpful when processing attributes with a mixture of
 * dynamic and non-dynamic storage duration. For attributes parsed
 * from an octet stream, or copied from another attribute list,
 * ownership necessarily passes to the list.
 */
class attribute_list {
private:
	/** All attributes in this list. */
	std::vector<const attribute*> _attributes;

	/** The attributes owned by this list. */
	std::vector<const attribute*> _owned_attributes;
public:
	attribute_list() = default;
	virtual ~attribute_list();

	attribute_list(const attribute_list&);
	attribute_list(attribute_list&& that);

	attribute_list& operator=(const attribute_list&);
	attribute_list& operator=(attribute_list&&);

	/** Build attribute list from octet reader.
	 * The length field must already have been read. This constructor
	 * must read exactly the specified number of octets.
	 * @param session the applicable session context
	 * @param length the length of the content, in octets
	 * @param in the octet reader
	 */
	attribute_list(session_context& session, size_t length,
		octet_reader& in);

	/** Determine whether this attribute list is empty.
	 * @return true if empty, otherwise false
	 */
	bool empty() const {
		return _attributes.empty();
	}

	/** Get the encoded length of the content of this list.
	 * The result includes a type and length field for each attribute,
	 * but not for the list as a whole.
	 * @return the content length, in octets
	 */
	size_t length() const;

	/** Get iterator for start of list.
	 * @return the iterator
	 */
	auto begin() const {
		return _attributes.begin();
	}

	/** Get iterator for end of list.
	 * @return the iterator
	 */
	auto end() const {
		return _attributes.end();
	}

	/** Get this attribute list as a standard container.
	 * @return the container
	 */
	const std::vector<const attribute*>& attributes() const {
		return _attributes;
	}

       /** Determine whether this list contains any instances of a given
         * attribute type.
         * @param type the required attribute type
         * @return true if there are one or more instances, otherwise false
         */
        bool contains(int type) const;
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
	 * A type and length field are written for each attribute, but not
	 * for the list as a whole.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Write attributes in human-readable form to an output stream.
	 * @param out the stream to which the output should be written
	 * @param attrlist the list of attributes to be written
	 * @return a reference to the output stream
	 */
	friend std::ostream& operator<<(std::ostream& out,
		const attribute_list& attrlist);
};

} /* namespace horace */

#endif
