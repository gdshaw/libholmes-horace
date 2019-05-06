// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/record.h"
#include "horace/session_writer.h"

namespace horace {

session_writer::session_writer(const std::string& source_id):
	_source_id(source_id) {}

void session_writer::write(const record& rec) {
	if (rec.type() >= record::REC_EVENT_MIN) {
		handle_event(rec);
	}
}

} /* namespace horace */
