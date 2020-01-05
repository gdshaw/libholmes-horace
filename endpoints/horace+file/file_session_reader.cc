// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <unistd.h>
#include <fcntl.h>

#include "horace/libc_error.h"
#include "horace/eof_error.h"
#include "horace/horace_error.h"
#include "horace/endpoint_error.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/attribute_list.h"

#include "filestore_scanner.h"
#include "spoolfile.h"
#include "spoolfile_reader.h"
#include "file_endpoint.h"
#include "file_session_reader.h"

namespace horace {

file_session_reader::file_session_reader(file_endpoint& src_ep,
	const std::string& srcid):
	_src_ep(&src_ep),
	_pathname(src_ep.pathname() + "/" + srcid),
	_dm(_pathname),
	_fd(_pathname, O_RDONLY),
	_watcher(_pathname),
	_lockfile(_pathname + "/.rdlock"),
	_next_filenum(0),
	_minwidth(0),
	_session_ts({0}),
	_seqnum(0) {}

std::string file_session_reader::_next_pathname() {
	// Construct the filename for the new spoolfile, incrementing the
	// current filenum.
	spoolfile sf(_next_filenum++, _minwidth);

	// If this caused the filenum to overflow then roll back the filenum
	// and throw an exception.
	if (_next_filenum == 0) {
		--_next_filenum;
		throw endpoint_error("file number overflow");
	}

	// Construct the full pathname for the new spoolfile.
	return _pathname + "/" + sf.filename();
}

std::unique_ptr<record> file_session_reader::read() {
	// If no spoolfile has been opened yet then attempt to open one.
	if (!_sfr) {
		// First scan to find first filenum. If the filestore is
		// empty then wait until at least one spoolfile is available.
		while (_minwidth == 0) {
			filestore_scanner scanner(_pathname);
			if (scanner.minwidth() != 0) {
				_next_filenum = scanner.first_filenum();
				_minwidth = scanner.minwidth();
			} else {
				wait();
			}
		}

		// Now open the spoolfile.
		std::string init_pathname = _next_pathname();
		_sfr = std::make_unique<spoolfile_reader>(*this,
			init_pathname, _next_pathname());
	}

	// If a sync record has already been returned for the current
	// spoolfile then it is an error to read any more records until
	// it has been acknowledged.
	if (_syncrec) {
		throw horace_error("ack record expected");
	}

	// Attempt to read a record, but be prepared for a possible
	// end of file error.
	try {
		std::unique_ptr<record> rec = std::make_unique<record>(_session, *_sfr);
		if (rec->channel_number() == channel_session) {
			struct timespec new_ts = rec->find_one<timestamp_attribute>(
				attrid_ts_begin).content();
			if ((new_ts.tv_sec != _session_ts.tv_sec) ||
				(new_ts.tv_nsec != _session_ts.tv_nsec)) {

				_session_ts = new_ts;
				_seqnum = 0;
			}
		} else if (rec->is_event()) {
			_seqnum = rec->find_one<unsigned_integer_attribute>(
				attrid_seqnum).content() + 1;
		}
		return rec;
	} catch (eof_error& ex) {
		// If there is no prospect of further data being read from
		// the current spoolfile (because the end has been reached
		// and a subsequent spoolfile has been detected) then
		// return a sync record.
		attribute_list attrs;
		attrs.append(std::make_unique<timestamp_attribute>(attrid_ts_begin, _session_ts));
		attrs.append(std::make_unique<unsigned_integer_attribute>(attrid_seqnum, _seqnum));
		_syncrec = std::make_unique<record>(channel_sync, std::move(attrs));
		return std::make_unique<record>(*_syncrec);
	}
}

void file_session_reader::write(const record& rec) {
	// The only type of record which should be written is an
	// acknowledgement record.
	if (rec.channel_number() != channel_sync) {
		throw horace_error("unexpected record type sent to session reader");
	}

	// Furthermore, acknowledgement records should only be written
	// in response to a sync record.
	if (!_syncrec) {
		throw horace_error("unexpected sync response sent to session reader");
	}

	// Check that the sync response record matches the outstanding
	// sync request.
	if (rec != *_syncrec) {
		throw horace_error("sync response record does not match sync request");
	}

	// Delete the current spoolfile, unless deletion suppressed.
	if (!_src_ep->nodelete()) {
		_sfr->unlink();
		_fd.fsync();
	}

	// Proceed to the next spoolfile.
	_sfr = std::make_unique<spoolfile_reader>(*this,
		_sfr->next_pathname(), _next_pathname());
	_syncrec = 0;
}

bool file_session_reader::reset() {
	if (_sfr) {
		_sfr = 0;
		_next_filenum -= 1;
		_session_ts = {0};
		_seqnum = 0;
		_syncrec = 0;
	}
	return true;
}

void file_session_reader::wait() {
	_watcher.read();
}

} /* namespace horace */
