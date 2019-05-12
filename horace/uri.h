// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_URI
#define LIBHOLMES_HORACE_URI

#include <optional>
#include <string>

namespace horace {

/** A base class to represent a uniform resource identifier (URI). */
class uri {
private:
	/** The scheme component. */
	std::optional<std::string> _scheme;

	/** The authority component. */
	std::optional<std::string> _authority;

	/** The path component. */
	std::string _path;

	/** The query component. */
	std::optional<std::string> _query;

	/** The fragment component. */
	std::optional<std::string> _fragment;
public:
	/** Construct URI.
	 * @param uri_string the URI as a string
	 */
	explicit uri(const std::string& uri_string);

	/** Convert URI back to string.
	 * @return the URI as a string
	 */
	operator std::string() const;

	/** Get the scheme component for this URI.
	 * @return the scheme component
	 */
	std::optional<std::string> scheme() const {
		return _scheme;
	}

	/** Get the authority component for this URI.
	 * @return the authority component
	 */
	std::optional<std::string> authority() const {
		return _authority;
	}

	/** Get the path component for this URI.
	 * @return the path component
	 */
	std::string path() const {
		return _path;
	}

	/** Get the query component for this URI.
	 * @return the query component
	 */
	std::optional<std::string> query() const {
		return _query;
	}

	/** Get the fragment component for this URI.
	 * @return the fragment component
	 */
	std::optional<std::string> fragment() const {
		return _fragment;
	}
};

} /* namespace horace */

#endif
