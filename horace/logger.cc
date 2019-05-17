// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/logger.h"

namespace horace {

logger::logger():
	_severity(log_warning) {}

logger& logger::operator<<(const std::string& msg) {
	std::lock_guard<std::mutex> lock(_log_mutex);
	write(msg);
}

std::unique_ptr<logger> log;

} /* namespace horace */
