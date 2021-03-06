// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_BUILDER
#define LIBHOLMES_HORACE_SESSION_BUILDER

#include <memory>
#include <string>

#include "horace/source_id.h"
#include "horace/attribute_list.h"

namespace horace {

class hash;
class keypair;
class record;

/** A class for building session records. */
class session_builder {
private:
	/** The source ID for this session. */
	source_id _srcid;

	/** An attribute list for the session record. */
	attribute_list _attributes;

	/** The number of attribute types defined. */
	int _attr_count;

	/** The number of channels defined. */
	int _chan_count;
public:
	/** Construct session builder.
	 * @param srcid the required source identifier
	 * @param time_system the required time system
	 */
	session_builder(const std::string& srcid,
		const std::string& time_system);

	/** Define attribute.
	 * If a label and format are requested which match a previous
	 * definition then this function is permitted but not required
	 * to reuse it.
	 * @param label the required attribute label
	 * @param fmt the required attribute format
	 */
	int define_attribute(const std::string& label,
		int fmt);

	/** Define channel.
	 * Invokation of this function always causes a new channel to be
	 * defined with a distinct channel number, regardless of whether
	 * an equivalent one already exists.
	 * @param label the required channel label
	 * @param subattrs a list of additional attributes
	 * @return the allocated channel number
	 */
	int define_channel(const std::string& label,
		attribute_list&& subattrs);

	/** Define hash function.
	 * @param hash the hash function
	 */
	void define_hash(const hash& hashfn);

	/** Define signature algorithm and public key.
	 * @param keypair the signing keypair
	 */
	void define_keypair(const keypair& kp);

	/** Build session record.
	 * @return the resulting record
	 */
	std::unique_ptr<record> build();
};

} /* namespace horace */

#endif
