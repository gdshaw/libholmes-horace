// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_SESSION_CONTEXT
#define LIBHOLMES_HORACE_SESSION_CONTEXT

#include <map>
#include <string>

namespace horace {

class compound_attribute;

/** A class for tracking the context of a HORACE session. */
class session_context {
private:
	/** The attribute labels for this session, indexed by code. */
	std::map<int, std::string> _attr_labels;

	/** The attribute formats for this session, indexed by code. */
	std::map<int, int> _attr_formats;

	/** The channel labels for this session, indexed by number. */
	std::map<int, std::string> _chan_labels;
public:
	/** Construct session context. */
	session_context();

	/** Handle an attribute type definition.
	 * @param attr the attribute to be handled
	 */
	void handle_attr_type_def(const compound_attribute& attr);

	/** Handle a channel definition.
	 * @param attr the attribute to be handled
	 */
	void handle_channel_def(const compound_attribute& attr);

	/** Get the attribute label for a given type code.
	 * @param type_code the required type code
	 * @return the corresponding attribute label
	 */
	const std::string& get_attr_label(int type_code);

	/** Get the attribute format for a given type code.
	 * @param type_code the required type code
	 * @return the corresponding attribute format
	 */
	int get_attr_format(int type_code);

	/** Get the channel label for a given channel number.
	 * @param channel_num the required channel number
	 * @return the corresponding channel label
	 */
	const std::string& get_channel_label(int channel_num);
};

} /* namespace horace */

#endif
