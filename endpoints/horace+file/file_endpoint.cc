// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <optional>

#include <fcntl.h>

#include "horace/query_string.h"
#include "horace/record.h"

#include "spoolfile_writer.h"
#include "file_session_listener.h"
#include "file_session_writer.h"
#include "file_endpoint.h"

namespace horace {

/** The default size to which spoolfiles are permitted to grow, in octets. */
static size_t default_filesize = 0x1000000;

file_endpoint::file_endpoint(const std::string& name):
	endpoint(name),
	_pathname(this->name().path()),
	_fd(_pathname, O_RDONLY),
	_filesize(default_filesize),
	_nodelete(false) {

	if (std::optional<std::string> query = this->name().query()) {
		query_string params(*query);
		_filesize = params.find<long>("filesize").value_or(_filesize);
		_nodelete = params.find<bool>("nodelete").value_or(_nodelete);
	}
}

std::unique_ptr<session_listener> file_endpoint::make_session_listener() {
	std::unique_ptr<session_listener> listener =
		std::make_unique<file_session_listener>(*this);
	_fd.fsync();
	return listener;
}

std::unique_ptr<session_writer> file_endpoint::make_session_writer(
	const std::string& srcid) {

	std::unique_ptr<session_writer> writer =
		std::make_unique<file_session_writer>(*this, srcid);
	_fd.fsync();
	return writer;
}

} /* namespace horace */

extern "C"
std::unique_ptr<horace::endpoint> make_endpoint(const std::string& name) {
	return std::make_unique<horace::file_endpoint>(name);
}
