// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_STRERR_LOGGER
#define LIBHOLMES_HORACE_STRERR_LOGGER

#include "horace/logger.h"

namespace horace {

/** A log writer class which writes to stderr. */
class stderr_logger:
	public logger {
protected:
	virtual void write(const std::string& msg);
};

} /* namespace horace */

#endif
