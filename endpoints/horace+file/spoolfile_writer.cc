// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <fcntl.h>

#include "horace/logger.h"
#include "horace/log_message.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/attribute_list.h"
#include "horace/record.h"

#include "spoolfile_writer.h"

namespace horace {

spoolfile_writer::spoolfile_writer(const std::string& pathname,
	size_t capacity):
	_pathname(pathname),
	_fd(pathname, O_RDWR|O_CREAT|O_EXCL, 0644),
	_ow(_fd),
	_size(0),
	_capacity(capacity),
	_seqnum(0) {

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "created spoolfile " << _pathname;
	}
}

void spoolfile_writer::sync() const {
	_fd.fsync();

	if (log->enabled(logger::log_info)) {
		log_message msg(*log, logger::log_info);
		msg << "synced spoolfile " << _pathname;
	}
}

bool spoolfile_writer::write(const record& rec) {
	// Calculate the number of octets required for this record,
	// including the channel and length fields.
	size_t content_len = rec.attributes().length();
	size_t full_len = octet_writer::signed_base128_length(rec.channel_number()) +
		octet_writer::unsigned_base128_length(content_len) + content_len;

	// Return false if this record would cause the spoolfile capacity
	// to be exceeded, except that it is always permissible to write
	// at least one event record to each spoolfile.
	if ((_size + full_len > _capacity) && (_seqnum != 0)) {
		return false;
	}

	// Write the record, updating the spoolfile size and seqnum.
	rec.write(_ow);
	_size += full_len;
	_seqnum = rec.update_seqnum(_seqnum) + 1;
	return true;
}

bool spoolfile_writer::write(uint64_t seqnum, const record& rec) {
	if (seqnum != _seqnum) {
		attribute_list attrs = rec.attributes();
		unsigned_integer_attribute seqnum_attr(attrid_seqnum, seqnum);
		attrs.append(seqnum_attr);
		record nrec(rec.channel_number(), std::move(attrs));
		return write(nrec);
	} else {
		return write(rec);
	}
}

} /* namespace horace */
