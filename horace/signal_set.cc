// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/libc_error.h"
#include "horace/signal_set.h"

namespace horace {

signal_set::signal_set():
	_sigset(std::make_unique<sigset_t>()) {

	sigemptyset(_sigset.get());
}

void signal_set::mask() const {
	if (int err = pthread_sigmask(SIG_BLOCK, _sigset.get(), NULL)) {
		throw libc_error(err);
	}
}

int signal_set::wait() const {
	int raised = 0;
	if (int err = sigwait(_sigset.get(), &raised)) {
		throw libc_error(err);
	}
	return raised;
}

signal_set terminating_signals = signal_set()
	.add(SIGINT)
	.add(SIGQUIT)
	.add(SIGTERM);

signal_set masked_signals = signal_set()
	.add(SIGINT)
	.add(SIGQUIT)
	.add(SIGALRM)
	.add(SIGTERM);

} /* namespace horace */
