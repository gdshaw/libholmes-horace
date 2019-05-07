// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/libc_error.h"
#include "horace/empty_signal_handler.h"

namespace horace {

void empty_signal_handler::_handle_signal(int signum) {}

empty_signal_handler::empty_signal_handler(int signum):
	_signum(signum) {

	struct sigaction action = {0};
	action.sa_handler = &_handle_signal;
	action.sa_flags = 0;
	if (sigaction(signum, &action, &_old_action) == -1) {
		throw libc_error();
	}
}

empty_signal_handler::~empty_signal_handler() {
	sigaction(_signum, &_old_action, 0);
}

} /* namespace horace */
