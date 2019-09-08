// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/horace_error.h"
#include "horace/unsigned_integer_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/string_attribute.h"
#include "horace/binary_ref_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"

#include "mongodb_error.h"
#include "mongodb_endpoint.h"
#include "mongodb_session_writer.h"

namespace horace {

void mongodb_session_writer::_append_bson(bson_t& bson, const attribute& attr) {
	if (const unsigned_integer_attribute* _attr = dynamic_cast<const unsigned_integer_attribute*>(&attr)) {
		bson_append_int64(&bson, _attr->name().c_str(), -1, _attr->content());
	} else if (const string_attribute* _attr = dynamic_cast<const string_attribute*>(&attr)) {
		bson_append_utf8(&bson, _attr->name().c_str(), -1, _attr->content().c_str(), -1);
	} else if (const binary_ref_attribute* _attr = dynamic_cast<const binary_ref_attribute*>(&attr)) {
		bson_append_binary(&bson, _attr->name().c_str(), -1, BSON_SUBTYPE_BINARY,
			reinterpret_cast<const uint8_t*>(_attr->content()), _attr->length());
	} else if (const timestamp_attribute* _attr = dynamic_cast<const timestamp_attribute*>(&attr)) {
		bson_t bson_ts;
		bson_append_document_begin(&bson, _attr->name().c_str(), -1, &bson_ts);
		bson_append_int64(&bson_ts, "sec", -1, _attr->content().tv_sec);
		bson_append_int32(&bson_ts, "nsec", -1, _attr->content().tv_nsec);
		bson_append_document_end(&bson, &bson_ts);
	} else if (const compound_attribute* _attr = dynamic_cast<const compound_attribute*>(&attr)) {
		bson_t bson_compound;
		bson_append_document_begin(&bson, _attr->name().c_str(), -1, &bson_compound);
		for (auto subattr : _attr->content().attributes()) {
			_append_bson(bson_compound, *subattr);
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
	_bulk_type = 0;
}

void mongodb_session_writer::_write_bulk(int type, const std::string& type_name, const bson_t& doc) {
	if (_bulk && (_bulk_type != type)) {
		sync();
	}

	if (!_bulk) {
		mongodb_collection& coll = _database.collection(type_name);
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

void mongodb_session_writer::handle_session_start(const session_start_record& srec) {
	_session_ts = srec.timestamp().content();
	_seqnum = 0;

	bson_t bson_session;
	bson_init(&bson_session);

	bson_t bson_id;
	bson_append_document_begin(&bson_session, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, source_id().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_document_end(&bson_session, &bson_id);

	bson_t bson_interfaces;
	bson_append_document_begin(&bson_session, "netif", -1, &bson_interfaces);
	for (auto netif_attr : srec.interfaces()) {
		std::string ifname = netif_attr->content().
			find_one<string_attribute>(attribute::ATTR_IFNAME).content();
		bson_t bson_interface;
		bson_append_document_begin(&bson_interfaces, ifname.c_str(),
			-1, &bson_interface);
		for (auto attr : netif_attr->content().attributes()) {
			_append_bson(bson_interface, *attr);
		}
		bson_append_document_end(&bson_interfaces, &bson_interface);
	}
	bson_append_document_end(&bson_session, &bson_interfaces);

	bson_error_t error;
	if (!mongoc_collection_insert_one(*_sessions, &bson_session, &_opts_session, 0, &error)) {
		if (error.code != 11000) {
			bson_destroy(&bson_session);
			throw mongodb_error(error);
		}
	}

	bson_destroy(&bson_session);
}

void mongodb_session_writer::handle_session_end(const session_end_record& erec) {
	struct timespec end_ts = erec.timestamp().content();

	bson_t bson_session;
	bson_t bson_id;
	bson_init(&bson_session);
	bson_append_document_begin(&bson_session, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, source_id().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_document_end(&bson_session, &bson_id);

	bson_t bson_update;
	bson_t bson_set;
	bson_init(&bson_update);
	bson_append_document_begin(&bson_update, "$set", -1, &bson_set);
	bson_t bson_ts2;
	bson_append_document_begin(&bson_set, "end_ts", -1, &bson_ts2);
	bson_append_int64(&bson_ts2, "sec", -1, end_ts.tv_sec);
	bson_append_int32(&bson_ts2, "nsec", -1, end_ts.tv_nsec);
	bson_append_document_end(&bson_set, &bson_ts2);
	bson_append_document_end(&bson_update, &bson_set);

	bson_error_t error;
	if (!mongoc_collection_update_one(*_sessions, &bson_session,
		&bson_update, 0, 0, &error)) {
		throw mongodb_error(error);
	}
}

void mongodb_session_writer::handle_sync(const sync_record& crec) {
	_sync();
}

void mongodb_session_writer::handle_event(const record& rec) {
	_seqnum = rec.update_seqnum(_seqnum);

	// Construct event with _id field (which must always be present).
	bson_t bson_event;
	bson_init(&bson_event);

	// Add _id field.
	bson_t bson_id;
	bson_append_document_begin(&bson_event, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, source_id().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_int64(&bson_id, "seqnum", -1, _seqnum);
	bson_append_document_end(&bson_event, &bson_id);

	// Add data from event record.
	for (auto subattr : rec.attributes()) {
		_append_bson(bson_event, *subattr);
	}

	// Append to the next bulk-write operation.
	_write_bulk(rec.type(), rec.type_name(), bson_event);
	bson_destroy(&bson_event);

	// Increment the sequence number.
	_seqnum += 1;
}

mongodb_session_writer::mongodb_session_writer(const mongodb_endpoint& dst_ep,
	const std::string& source_id):
	simple_session_writer(source_id),
	_database(dst_ep.name()),
	_sessions(&_database.collection("sessions")),
	_seqnum(0),
	_bulk(0),
	_bulk_count(0),
	_bulk_type(0) {

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
