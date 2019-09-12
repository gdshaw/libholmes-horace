// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_EVENT_READER
#define LIBHOLMES_HORACE_EVENT_READER

#include "horace/counter.h"

namespace horace {

class record;
class record_builder;
class filter;

/** A class for reading events from an endpoint.
 * This class is intended for use by endpoints which act as a source of
 * newly-captured events which have not yet been packaged into sessions
 * (and which can therefore be mixed with events from other endpoints).
 *
 * Endpoints which include the mixin class event_reader_endpoint have the
 * ability to create an instance of this class. That instance then allows
 * events to be read from the endpoint.
 */
class event_reader {
public:
	virtual ~event_reader() = default;

	/** Read an event.
	 * If there is no event immediately available then this function
	 * will block until one can be read.
	 *
	 * The resulting event record remains valid until either this
	 * function is called again in respect of same event reader, or
	 * the event reader is destroyed.
	 * @return the resulting event record
	 */
	virtual const record& read() = 0;

	/** Attach a BPF filter to this event reader.
	 * Event readers which do not capture network packets should take no
	 * action when this function is called. This is the default behaviour.
	 * @param filt the filter to be attached
	 */
	virtual void attach(const filter& filt);

	/** Build session start record.
	 * This function gives each active event reader the opportunity to
	 * contribute attributes to the session start record which are
	 * specific to that event reader. For example, an event reader which
	 * captures traffic from a network interface would add a
	 * netif_attribute to the record.
	 *
	 * Note that implementations of this function are not responsible
	 * for adding the source attribute, the session timestamp attribute,
	 * or any other attribute which is not associated with a specific
	 * event reader. The default implementation does nothing.
	 * @param builder a record builder for the session start record
	 */
	virtual void build_session_start(record_builder& builder);
};

} /* namespace horace */

#endif
