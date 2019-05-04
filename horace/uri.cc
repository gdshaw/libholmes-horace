// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/uri.h"

namespace horace {

uri::uri(const std::string& uri_string) {
	// Parse scheme component.
	size_t index = 0;
	size_t scheme_end = uri_string.find(':', index);
	if (scheme_end != std::string::npos) {
		_scheme = uri_string.substr(index, scheme_end - index);
		index = scheme_end + 1;
	}

	// Parse authority component.
	if ((index + 2 <= uri_string.length()) &&
		(uri_string[index + 0] == '/') &&
		(uri_string[index + 1] == '/')) {

		index += 2;
		size_t authority_end = uri_string.find_first_of("/?#", index);
		if (authority_end == std::string::npos) {
			authority_end = uri_string.length();
		}
		_authority = uri_string.substr(index, authority_end - index);
		index = authority_end;
	}

	// Parse path component.
	size_t path_end = uri_string.find_first_of("?#", index);
	if (path_end == std::string::npos) {
		path_end = uri_string.length();
	}
	_path = uri_string.substr(index, path_end - index);
	index = path_end;

	// Parse query component, if there is one.
	if ((index < uri_string.length()) && (uri_string[index] == '?')) {
		// Query component is present.
		index += 1;
		size_t query_end = uri_string.find('#', index);
		if (query_end == std::string::npos) {
			query_end = uri_string.length();
		}
		_query = uri_string.substr(index, query_end - index);
		index = query_end;
	}

	// Parse fragment component, if there is one.
	if ((index < uri_string.length()) && (uri_string[index] == '#')) {
		// Fragment component is present.
		index += 1;
		_fragment = uri_string.substr(index);
		index = uri_string.length();
	}

	// If the end of the URI has not been reached yet then
	// that indicates an error in the parsing algorithm.
	if (index != uri_string.length()) {
		throw std::logic_error("internal error while parsing URI");
	}
}

} /* namespace horace */
