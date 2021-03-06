// This file is part of libholmes.
// Copyright 2019-20 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include <map>
#include <list>

#include "horace/horace_error.h"
#include "horace/compound_attribute.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/signed_integer_attribute.h"
#include "horace/binary_ref_attribute.h"
#include "horace/string_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/boolean_attribute.h"
#include "horace/unrecognised_attribute.h"
#include "horace/record.h"

#include "mongodb_error.h"
#include "mongodb_collection.h"
#include "mongodb_endpoint.h"
#include "mongodb_session_writer.h"

namespace horace {

void mongodb_session_writer::_append_bson(bson_t& bson, const attribute& attr) {
	_append_bson(bson, _session.get_attr_label(attr.attrid()), attr);
}

void mongodb_session_writer::_append_bson(bson_t& bson, unsigned int index, const attribute& attr) {
	_append_bson(bson, std::to_string(index), attr);
}

void mongodb_session_writer::_append_bson(bson_t& bson, const std::string& attr_label, const attribute& attr) {
	if (const unsigned_integer_attribute* _attr = dynamic_cast<const unsigned_integer_attribute*>(&attr)) {
		bson_append_int64(&bson, attr_label.c_str(), -1, _attr->content());
	} else if (const signed_integer_attribute* _attr = dynamic_cast<const signed_integer_attribute*>(&attr)) {
		bson_append_int64(&bson, attr_label.c_str(), -1, _attr->content());
	} else if (const string_attribute* _attr = dynamic_cast<const string_attribute*>(&attr)) {
		bson_append_utf8(&bson, attr_label.c_str(), -1, _attr->content().c_str(), -1);
	} else if (const binary_ref_attribute* _attr = dynamic_cast<const binary_ref_attribute*>(&attr)) {
		bson_append_binary(&bson, attr_label.c_str(), -1, BSON_SUBTYPE_BINARY,
			reinterpret_cast<const uint8_t*>(_attr->content()), _attr->length());
	} else if (const timestamp_attribute* _attr = dynamic_cast<const timestamp_attribute*>(&attr)) {
		bson_t bson_ts;
		bson_append_document_begin(&bson, attr_label.c_str(), -1, &bson_ts);
		bson_append_int64(&bson_ts, "sec", -1, _attr->content().tv_sec);
		bson_append_int32(&bson_ts, "nsec", -1, _attr->content().tv_nsec);
		bson_append_document_end(&bson, &bson_ts);
	} else if (const boolean_attribute* _attr = dynamic_cast<const boolean_attribute*>(&attr)) {
		bson_append_bool(&bson, attr_label.c_str(), -1, _attr->content());
	} else if (const compound_attribute* _attr = dynamic_cast<const compound_attribute*>(&attr)) {
		std::map<std::string, std::list<const attribute*>> attr_lists;
		for (const auto& subattr : _attr->content().attributes()) {
			std::string subattr_label = _session.get_attr_label(subattr->attrid());
			attr_lists[subattr_label].push_back(subattr);
		}

		bson_t bson_compound;
		bson_append_document_begin(&bson, attr_label.c_str(), -1, &bson_compound);
		for (const auto& pair : attr_lists) {
			if (pair.second.size() == 1) {
				_append_bson(bson_compound, *pair.second.front());
			} else {
				bson_t bson_compound_array;
				bson_append_array_begin(&bson_compound, pair.first.c_str(), -1, &bson_compound_array);
				unsigned int i = 0;
				for (const auto& subattr : pair.second) {
					_append_bson(bson_compound_array, i++, *subattr);
				}
				bson_append_array_end(&bson_compound, &bson_compound_array);
			}
		}
		for (const auto& subattr : _attr->content().attributes()) {
		}
		bson_append_document_end(&bson, &bson_compound);
	} else {
		throw horace_error("cannot convert attribute to BSON");
	}
}

void mongodb_session_writer::_sync() {
	if (!_bulk) {
		return;
	}

	bson_t reply;
	bson_error_t error;
	if (mongoc_bulk_operation_execute(_bulk, &reply, &error) == 0) {
		throw mongodb_error(error);
	}

	bson_destroy(&reply);
	mongoc_bulk_operation_destroy(_bulk);
	_bulk = 0;
	_bulk_count = 0;
	_bulk_channel = 0;
}

void mongodb_session_writer::_write_bulk(int channel_id,
	const std::string& channel_label, const bson_t& doc) {

	if (_bulk && (_bulk_channel != channel_id)) {
		sync();
	}

	if (!_bulk) {
		mongodb_collection& coll = _database.collection(channel_label);
		_bulk = mongoc_collection_create_bulk_operation_with_opts(coll, &_opts_bulk);
	}

	bson_error_t error;
	if (!mongoc_bulk_operation_replace_one_with_opts(_bulk, &doc, &doc, &_opts_event, &error)) {
		throw mongodb_error(error);
	}
	_bulk_count += 1;
	// Until there is a mechanism for triggering a sync, it is unsafe
	// to perform more than one insertion in the bulk operation.
	// This defeats the object of using a bulk operation as it stands,
	// but is not expected to be a permanent state of affairs.
	if (_bulk_count >= 1) {
		_sync();
	}
}

void mongodb_session_writer::handle_session_start(const record& srec) {
	_session_ts = srec.find_one<timestamp_attribute>(
		attrid_ts).content();
	_session = session_context();

	bson_t bson_session;
	bson_init(&bson_session);
	bson_t bson_id;
	bson_append_document_begin(&bson_session, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, srcid().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_document_end(&bson_session, &bson_id);

	bson_t bson_channels;
	bson_init(&bson_channels);
	for (const auto& attr : srec.attributes()) {
		if (attr->attrid() == attrid_attr_def) {
			_session.handle_attr_def(
				dynamic_cast<const compound_attribute&>(*attr));
			continue;
		} else if (attr->attrid() != attrid_chan_def) {
			_append_bson(bson_session, *attr);
			continue;
		}

		const compound_attribute& channel_def =
			dynamic_cast<const compound_attribute&>(*attr);
		_session.handle_channel_def(channel_def);

		int64_t channel_id = channel_def.content().
			find_one<signed_integer_attribute>(attrid_chan_id).content();
		std::string channel_label = channel_def.content().
			find_one<string_attribute>(attrid_chan_label).content();

		std::string channel_str = std::string("channel") + std::to_string(channel_id);
		bson_t bson_channel;
		bson_append_document_begin(&bson_channels, channel_str.c_str(),
			-1, &bson_channel);
		for (const auto& subattr : channel_def.content().attributes()) {
			_append_bson(bson_channel, *subattr);
		}
		bson_append_document_end(&bson_channels, &bson_channel);
	}
	bson_append_document(&bson_session, "channels", -1, &bson_channels);

	mongoc_cursor_t* cursor = mongoc_collection_find_with_opts(*_sessions,
		&bson_session, 0, 0);
	const bson_t* result = 0;
	bool found = mongoc_cursor_next(cursor, &result);
	if (found) {
		// Note: does not detect the case where the session record
		// contains less than when originally recorded.
		mongoc_cursor_destroy(cursor);
		bson_destroy(&bson_session);
		return;
	}
	mongoc_cursor_destroy(cursor);

        bson_error_t error;
	if (!mongoc_collection_insert_one(*_sessions, &bson_session,
		&_opts_session, 0, &error)) {

		bson_destroy(&bson_session);
		throw mongodb_error(error);
	}

	bson_destroy(&bson_session);
}

void mongodb_session_writer::handle_session_end(const record& erec) {
        struct timespec ts = erec.find_one<timestamp_attribute>(
                attrid_ts).content();

	bson_t bson_filter;
	bson_init(&bson_filter);
	bson_t bson_id;
	bson_append_document_begin(&bson_filter, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, srcid().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_document_end(&bson_filter, &bson_id);

        bson_t bson_update;
        bson_init(&bson_update);
        bson_t bson_set;
        bson_append_document_begin(&bson_update, "$set", -1, &bson_set);
        bson_t bson_end;
        bson_append_document_begin(&bson_set, "_end", -1, &bson_end);
	for (const auto& subattr : erec.attributes()) {
		_append_bson(bson_end, *subattr);
	}
        bson_append_document_end(&bson_set, &bson_end);
        bson_append_document_end(&bson_update, &bson_set);

        bson_error_t error;
        if (!mongoc_collection_update_one(*_sessions, &bson_filter,
                &bson_update, 0, 0, &error)) {
                throw mongodb_error(error);
        }
}

void mongodb_session_writer::handle_sync(const record& crec) {
	_sync();
}

void mongodb_session_writer::handle_event(const record& rec) {
	uint64_t seqnum = rec.find_one<unsigned_integer_attribute>(
		attrid_seqnum).content();

	// Construct event with _id field (which must always be present).
	bson_t bson_event;
	bson_init(&bson_event);

	// Add _id field.
	bson_t bson_id;
	bson_append_document_begin(&bson_event, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, srcid().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_int64(&bson_id, "seqnum", -1, seqnum);
	bson_append_document_end(&bson_event, &bson_id);

	// Add data from event record.
	for (const auto& subattr : rec.attributes()) {
		_append_bson(bson_event, *subattr);
	}

	// Append to the next bulk-write operation.
	std::string label = _session.get_channel_label(rec.channel_id());
	_write_bulk(rec.channel_id(), label, bson_event);
	bson_destroy(&bson_event);
}

mongodb_session_writer::mongodb_session_writer(const mongodb_endpoint& dst_ep,
	const std::string& srcid):
	simple_session_writer(srcid),
	_database(dst_ep.name()),
	_sessions(&_database.collection("sessions")),
	_bulk(0),
	_bulk_count(0),
	_bulk_channel(0) {

	_wc = mongoc_write_concern_new();
	mongoc_write_concern_set_w(_wc, MONGOC_WRITE_CONCERN_W_MAJORITY);
	mongoc_write_concern_set_journal(_wc, true);

	bson_init(&_opts_session);
	mongoc_write_concern_append(_wc, &_opts_session);

	bson_init(&_opts_bulk);
	bson_append_bool(&_opts_bulk, "ordered", -1, false);
	mongoc_write_concern_append(_wc, &_opts_bulk);

	bson_init(&_opts_event);
	bson_append_bool(&_opts_event, "upsert", -1, true);
}

mongodb_session_writer::~mongodb_session_writer() {
	bson_destroy(&_opts_session);
	bson_destroy(&_opts_bulk);
	bson_destroy(&_opts_event);
	mongoc_write_concern_destroy(_wc);
}

} /* namespace horace */
