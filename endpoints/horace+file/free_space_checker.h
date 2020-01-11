// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_FREE_SPACE_CHECKER
#define LIBHOLMES_FREE_SPACE_CHECKER

namespace horace {

/** A class for checking whether a filesystem has sufficient free space
 * for data collection, according to user-specified thresholds.
 * There are two thresholds, a high water mark and a low water mark:
 * - When the occupied space is at or above the high water mark,
 *   collection stops.
 * - When it is below the high water mark, but at or above the low water
 *   mark, collection continues if it has already started, but will not
 *   start or restart if it is stopped.
 * - Below the low water mark, collection will start or restart.
 */
class free_space_checker {
private:
	/** A pathname within the filesystem to be checked. */
	std::string _pathname;

	/** The required high water mark, as a fraction of capacity. */
	double _hwm;

	/** The required low water mark, as a fraction of capacity. */
	double _lwm;

	/** The most recent result.
	 * This may affect the next result due to hysteresis.
	 */
	bool _result;
public:
	/** Calculate free space given pathname.
	 * Currently the high and low water marks must be specified as
	 * percentages of the available capacity.
	 * @param pathname a pathname within the filesystem to be checked
	 * @param hwm the required high water mark, as a string
	 * @param lwm the required low water mark, as a string
	 */
	free_space_checker(const std::string& pathname,
		const std::string& hwm, const std::string& lwm);

	/** Perform free space check.
	 * @return true if sufficient free space, otherwise false
	 */
	operator bool();
};

} /* namespace horace */

#endif
