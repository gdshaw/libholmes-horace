// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#include "horace/unsigned_integer_attribute.h"
#include "horace/timestamp_attribute.h"
#include "horace/compound_attribute.h"
#include "horace/session_start_record.h"
#include "horace/session_end_record.h"
#include "horace/packet_record.h"

#include "mongodb_error.h"
#include "mongodb_endpoint.h"
#include "mongodb_session_writer.h"

namespace horace {

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
}

void mongodb_session_writer::_write_bulk(const bson_t& doc) {
	if (!_bulk) {
		_bulk = mongoc_collection_create_bulk_operation_with_opts(_packets, &_opts_bulk);
	}

	bson_error_t error;
	if (!mongoc_bulk_operation_replace_one_with_opts(_bulk, &doc, &doc, &_opts_packet, &error)) {
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

void mongodb_session_writer::handle_packet(const packet_record& prec) {
	_seqnum = prec.update_seqnum(_seqnum);

	// Construct packet with _id field (which must always be present).
	bson_t bson_packet;
	bson_t bson_id;
	bson_init(&bson_packet);
	bson_append_document_begin(&bson_packet, "_id", -1, &bson_id);
	bson_append_utf8(&bson_id, "source", -1, source_id().c_str(), -1);
	bson_t bson_ts;
	bson_append_document_begin(&bson_id, "ts", -1, &bson_ts);
	bson_append_int64(&bson_ts, "sec", -1, _session_ts.tv_sec);
	bson_append_int32(&bson_ts, "nsec", -1, _session_ts.tv_nsec);
	bson_append_document_end(&bson_id, &bson_ts);
	bson_append_int64(&bson_id, "seqnum", -1, _seqnum);
	bson_append_document_end(&bson_packet, &bson_id);

	// Add timestamp field, if applicable.
	if (const timestamp_attribute* ts_attr = prec.timestamp()) {
		struct timespec packet_ts = ts_attr->content();
		bson_t bson_ts2;
		bson_append_document_begin(&bson_packet, "ts", -1, &bson_ts2);
		bson_append_int64(&bson_ts2, "sec", -1, packet_ts.tv_sec);
		bson_append_int32(&bson_ts2, "nsec", -1, packet_ts.tv_nsec);
		bson_append_document_end(&bson_packet, &bson_ts2);
	}

	// Add content and length fields, if applicable.
	// The length field can derive either from and origlen attribute,
	// or in its absence, the length of the content.
	if (const binary_ref_attribute* packet_attr = prec.packet_attr()) {
		const void* content = packet_attr->content();
		size_t snaplen = packet_attr->length();
		size_t origlen = snaplen;
		if (const unsigned_integer_attribute *origlen_attr = prec.origlen_attr()) {
			origlen = origlen_attr->content();
		}
		bson_append_binary(&bson_packet, "content", -1, BSON_SUBTYPE_BINARY,
			reinterpret_cast<const uint8_t*>(content), snaplen);
		bson_append_int64(&bson_packet, "length", -1, origlen);
	} else if (const unsigned_integer_attribute *origlen_attr = prec.origlen_attr()) {
		size_t origlen = origlen_attr->content();
		bson_append_int64(&bson_packet, "length", -1, origlen);
	}

	// Add repeat field, if applicable.
	if (const unsigned_integer_attribute* repeat_attr =
		dynamic_cast<const unsigned_integer_attribute*>(prec.repeat_attr())) {

		bson_append_int32(&bson_packet, "repeat", -1, repeat_attr->content());
	}

	// Append to the next bulk-write operation.
	_write_bulk(bson_packet);
	bson_destroy(&bson_packet);
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
			if (attr->type() == attribute::ATTR_LINKTYPE) {
				const unsigned_integer_attribute& linktype_attr =
					dynamic_cast<const unsigned_integer_attribute&>(*attr);
				bson_append_int32(&bson_interface, "linktype", -1,
					linktype_attr.content());
			} else if (attr->type() == attribute::ATTR_EUI) {
				const binary_ref_attribute& eui_attr =
					dynamic_cast<const binary_ref_attribute&>(*attr);
				bson_append_binary(&bson_interface, "hwaddr", -1,
					BSON_SUBTYPE_BINARY,
					reinterpret_cast<const uint8_t*>(eui_attr.content()),
					eui_attr.length());
			}
		}
		bson_append_document_end(&bson_interfaces, &bson_interface);
	}
	bson_append_document_end(&bson_session, &bson_interfaces);

	bson_error_t error;
	if (!mongoc_collection_insert_one(_sessions, &bson_session, &_opts_session, 0, &error)) {
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
	if (!mongoc_collection_update_one(_sessions, &bson_session,
		&bson_update, 0, 0, &error)) {
		throw mongodb_error(error);
	}
}

void mongodb_session_writer::handle_sync(const sync_record& crec) {
	_sync();
}

void mongodb_session_writer::handle_event(const record& rec) {
	if (const packet_record* prec =
		dynamic_cast<const packet_record*>(&rec)) {
		handle_packet(*prec);
	} else {
		throw endpoint_error("unsupported event record type");
	}

	// Any type of event causes the sequence number to increment.
	_seqnum += 1;
}

mongodb_session_writer::mongodb_session_writer(const mongodb_endpoint& dst_ep,
	const std::string& source_id):
	simple_session_writer(source_id),
	_database(dst_ep.name()),
	_sessions(_database.collection("sessions")),
	_packets(_database.collection("packets")),
	_seqnum(0),
	_bulk(0),
	_bulk_count(0) {

	_wc = mongoc_write_concern_new();
	mongoc_write_concern_set_w(_wc, MONGOC_WRITE_CONCERN_W_MAJORITY);
	mongoc_write_concern_set_journal(_wc, true);

	bson_init(&_opts_session);
	mongoc_write_concern_append(_wc, &_opts_session);

	bson_init(&_opts_bulk);
	bson_append_bool(&_opts_bulk, "ordered", -1, false);
	mongoc_write_concern_append(_wc, &_opts_bulk);

	bson_init(&_opts_packet);
	bson_append_bool(&_opts_packet, "upsert", -1, true);
}

mongodb_session_writer::~mongodb_session_writer() {
	bson_destroy(&_opts_session);
	bson_destroy(&_opts_bulk);
	bson_destroy(&_opts_packet);
	mongoc_write_concern_destroy(_wc);
}

} /* namespace horace */
