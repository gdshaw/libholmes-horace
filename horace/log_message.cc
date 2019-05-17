// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/logger.h"
#include "horace/log_message.h"

namespace horace {

log_message::log_message(logger& log, int severity):
	_log(&log),
	_severity(severity) {}

log_message::~log_message() {
	(*_log) << str();
}

} /* namespace horace */
