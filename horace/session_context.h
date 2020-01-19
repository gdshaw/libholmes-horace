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
	/** The attribute names for this session, indexed by ID. */
	std::map<int, std::string> _attr_names;

	/** The attribute formats for this session, indexed by ID. */
	std::map<int, int> _attr_fmts;

	/** The channel labels for this session, indexed by number. */
	std::map<int, std::string> _chan_labels;
public:
	/** Handle an attribute definition.
	 * @param attr the attribute definition attribute to be handled
	 */
	void handle_attr_def(const compound_attribute& attr);

	/** Handle a channel definition.
	 * @param attr the channel definition attribute to be handled
	 */
	void handle_channel_def(const compound_attribute& attr);

	/** Get the attribute name for a given attribute ID.
	 * @param attrid the required attribute ID
	 * @return the corresponding attribute name
	 */
	const std::string& get_attr_name(int attrid);

	/** Get the attribute format for a given attribute ID.
	 * @param attrid the required attribute ID
	 * @return the corresponding attribute format
	 */
	int get_attr_fmt(int attrid);

	/** Get the channel label for a given channel number.
	 * @param channel_num the required channel number
	 * @return the corresponding channel label
	 */
	const std::string& get_channel_label(int channel_num);
};

} /* namespace horace */

#endif
