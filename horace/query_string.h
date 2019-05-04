// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_QUERY_STRING
#define LIBHOLMES_HORACE_QUERY_STRING

#include <optional>
#include <map>
#include <string>

namespace horace {

/** A class to represent a decoded query string.
 * If two parameters have the same name then the later one
 * overwrites the earlier one.
 */
class query_string {
private:
	/** The query parameters. */
	std::map<std::string, std::string> _parameters;
public:
	/** Construct query string.
	 * @param query the encoded query string
	 */
	explicit query_string(const std::string& query);

	/** Find value of parameter.
	 * The required return type should be specified as a template
	 * parameter. The only types currently supported are:
	 * - bool (taking a string "true" or "false")
	 * - long (taking a decimal string)
	 * - std::string
	 * @param name the name of the parameter to be found
	 */
	template<typename T>
	std::optional<T> find(const std::string& name) const;
};

} /* namespace horace */

#endif
