// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_BUILDER
#define LIBHOLMES_HORACE_SESSION_BUILDER

#include <memory>
#include <string>

#include "horace/record_builder.h"

namespace horace {

class attribute_list;
class record;

/** A class for building start of session records. */
class session_builder {
private:
	/** The source ID for this session. */
	std::string _source_id;

	/** A record builder for the start of session record. */
	record_builder _srecb;

	/** The number of attribute types defineds. */
	int _attr_count;

	/** The number of channels defined. */
	int _chan_count;
public:
	/** Construct session builder.
	 * @param source_id the required source identifier
	 */
	explicit session_builder(const std::string& source_id);

	/** Get the source ID for this session.
	 * @return the source ID
	 */
	const std::string& source_id() const {
		return _source_id;
	}

	/** Define attribute type.
	 * If a type is requested which matches a previous definition
	 * then this function is permitted but not required to reuse it.
	 * @param label the required attribute label
	 * @param format the required attribute format
	 */
	int define_attribute_type(const std::string& label,
		int format);

	/** Define channel.
	 * Invokation of this function always causes a new channel to be
	 * defined with a distinct channel number, regardless of whether
	 * an equivalent one already exists.
	 * @param label the required channel label
	 * @param attrs a list of additional attributes
	 * @return the allocated channel number
	 */
	int define_channel(const std::string& label, attribute_list&& attrs);

	/** Build start of session record.
	 * @return the resulting record
	 */
	std::unique_ptr<record> build();
};

} /* namespace horace */

#endif
