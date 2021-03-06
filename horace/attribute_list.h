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
	 * The result includes an ID and length field for each attribute,
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
	const auto& attributes() const {
		return _attributes;
	}

       /** Determine whether this list contains any attributes with a given
         * attribute ID.
         * @param attrid the required attribute ID
         * @return true if there are one or more instances, otherwise false
         */
        bool contains(int attrid) const;
private:
	/** Find a single attribute with a given attribute ID.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param attrid the required attribute ID
	 */
	const attribute& _find_one(int attrid) const;
public:
	/** Find a single attribute with a given attribute ID.
	 * It is an error if there are no matching attributes, or if there
	 * is more than one matching attribute.
	 * @param attrid the required attribute ID
	 */
	template<class T>
	const T& find_one(int attrid) const {
		return dynamic_cast<const T&>(_find_one(attrid));
	}

	/** Insert an attribute to this list, with transfer of ownership.
	 * @param attr the attribute to be inserted
	 * @return a reference to this
	 */
	attribute_list& insert(std::unique_ptr<attribute>& attr);

	/** Insert an attribute to this list, with transfer of ownership.
	 * @param attr the attribute to be inserted
	 * @return a reference to this
	 */
	attribute_list& insert(std::unique_ptr<attribute>&& attr);

	/** Insert an attribute to this record, without transfer of ownership.
	 * It is the caller's responsibility to ensure that the attribute
	 * remains in existence until there is no further possibility of it
	 * being accessed.
	 * @param attr the attribute to be inserted
	 * @return a reference to this
	 */
	attribute_list& insert(const attribute& attr);

	/** Write this attribute list to an octet writer.
	 * An ID and length field are written for each attribute, but not
	 * for the list as a whole.
	 * @param out the octet writer
	 */
	void write(octet_writer& out) const;

	/** Test whether two attribute lists are equal.
	 * The attributes must match in value and order for the lists to
	 * compare as equal.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if equal, otherwise false
	 */
	friend bool operator==(const attribute_list& lhs,
		const attribute_list& rhs);

	/** Test whether two attribute lists are not equal.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if not equal, otherwise false
	 */
	friend bool operator!=(const attribute_list& lhs,
		const attribute_list& rhs) {

		return !(lhs == rhs);
	}

	/** Test whether one attribute list is a prefix of another.
	 * @param lhs the left hand side
	 * @param rhs the right hand side
	 * @return true if lhs is a prefix of rhs, otherwise false
	 */
	friend bool operator<=(const attribute_list& lhs,
		const attribute_list& rhs);

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
