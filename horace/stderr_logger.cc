// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <ctime>
#include <iostream>

#include "horace/stderr_logger.h"

namespace horace {

void stderr_logger::write(const std::string& msg) {
	time_t now = time(0);
	struct tm now_tm;
	gmtime_r(&now, &now_tm);
	char now_str[32];
	strftime(now_str, sizeof(now_str), "%Y-%m-%dT%H:%M:%SZ", &now_tm);
	std::cerr << now_str << ": " << msg << std::endl;
}

};
