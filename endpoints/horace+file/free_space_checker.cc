// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <sys/statvfs.h>

#include "horace/libc_error.h"
#include "horace/endpoint_error.h"

#include "free_space_checker.h"

namespace horace {

free_space_checker::free_space_checker(const std::string& pathname,
	const std::string& hwm, const std::string& lwm):
	_pathname(pathname),
	_result(false) {

	// Parse high water mark.
	int hwm_count = 0;
	double hwm_percent = 0;
	sscanf(hwm.c_str(), "%lf%%%n", &hwm_percent, &hwm_count);
	if (hwm_count != hwm.length()) {
		throw endpoint_error("hwm does not specify a percentage threshold");
	}
	_hwm = hwm_percent / 100.0;

	// Parse low water mark.
	int lwm_count = 0;
	double lwm_percent = 0;
	sscanf(lwm.c_str(), "%lf%%%n", &lwm_percent, &lwm_count);
	if (lwm_count != lwm.length()) {
		throw endpoint_error("lwm does not specify a percentage threshold");
	}
	_lwm = lwm_percent / 100.0;

	// Validate high and low water marks.
	if ((_hwm < 0.0) || (_hwm > 1.0)) {
		throw endpoint_error("hwm outside range 0-100%");
	}
	if ((_lwm < 0.0) || (_lwm > 1.0)) {
		throw endpoint_error("lwm outside range of 0-100%");
	}
	if (_lwm > _hwm) {
		throw endpoint_error("lwm exceeds hwm");
	}
}

free_space_checker::operator bool() {
	// Get statistics for the filesystem containing the pathname
	// from the endpoint URI.
	struct statvfs buf;
	if (statvfs(_pathname.c_str(), &buf) == -1) {
		throw libc_error();
	}

	// Calculate the fraction of blocks that have been used, out of
	// the number available to unprivileged users (so excluding any
	// reserved blocks).
	double used_blocks = buf.f_blocks - buf.f_bfree;
	double total_blocks = used_blocks + buf.f_bavail;
	double used_frac = used_blocks / total_blocks;

	// There are three possibilities:
	// - At or above the high water mark, collection is turned off
	//   regardless of its previous state.
	// - Below the low water mark it is turned on, again regardless
	//   of its previous state.
	// - Between the two marks is the hysteresis region where it is
	//   left unchanged.
	if (used_frac >= _hwm) {
		_result = false;
	} else if (used_frac < _lwm) {
		_result = true;
	}
	return _result;
}

} /* namespace horace */
