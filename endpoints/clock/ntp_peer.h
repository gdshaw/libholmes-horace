// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_PEER
#define LIBHOLMES_HORACE_NTP_PEER

#include "ntp_response.h"

namespace horace {

class ntp_assoc_attr_builder;

/** A class to represent the state of an NTP peer.
 * Escaping within quoted strings is not yet implemented.
 */
class ntp_peer {
private:
	/** The association ID. */
	uint16_t _associd;

	/** The status code. */
	uint16_t _status;

	/** The variables for this peer, indexed by name. */
	std::map<std::string, std::string> _variables;
public:
	/** Construct peer state from NTP response. */
	ntp_peer(const ntp_response& response);

	/** Test whether peer contains a given variable.
	 * @param name the name of the variable
	 * @return true if variable present, otherwise false
	 */
	bool contains(const std::string& varname);

	/** Get an unquoted string peer variable.
	 * @param name the name of the variable
	 * @return the value
	 */
	std::string get_string(const std::string& varname);

	/** Get a quoted string peer variable.
	 * @param name the name of the variable
	 * @return the value
	 */
	std::string get_quoted_string(const std::string& varname);

	/** Get an integer decimal peer variable
	 * @param name the name of the variable
	 * @return the value
	 */
	int64_t get_integer(const std::string& varname);

	/** Get a fixed-point decimal peer variable
	 * @param name the name of the variable
	 * @return the value
	 */
	int64_t get_fixed(const std::string& varname);

	/** Build NTP association attribute.
	 * @param builder an NTP association attribute builder object
	 */
	void build(ntp_assoc_attr_builder& builder);
};

} /* namespace horace */

#endif
