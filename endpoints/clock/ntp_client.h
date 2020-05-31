// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_CLIENT
#define LIBHOLMES_HORACE_NTP_CLIENT

#include <memory>
#include <map>

#include "horace/socket_descriptor.h"

#include "ntp_response.h"
#include "ntp_peer.h"

namespace horace {

/** A class for making mode 6 requests to an NTP server. */
class ntp_client {
private:
	/** The socket descriptor for communicating with the server. */
	socket_descriptor _sock;

	/** A buffer for receiving responses. */
	unsigned char _buffer[4096];

	/** The sequence number for the next request. */
	uint16_t _seqnum;

	/** The requests made during the current update cycle,
	 * indexed by sequence number. */
	std::map<uint16_t, std::unique_ptr<ntp_datagram>> _requests;

	/** The responses received during the current update cycle,
	 * indexed by sequence number. */
	std::map<uint16_t, std::unique_ptr<ntp_response>> _responses;

	/** The peer status objects collected during the current
	 * update cycle, indexed by association ID. */
	std::map<uint16_t, std::unique_ptr<ntp_peer>> _peers;
public:
	/** Construct NTP client. */
	ntp_client();

	/** Send an NTP datagram to the server.
	 * @param datagram the datagram to be sent
	 */
	void send(const ntp_datagram& datagram);

	/** Receive an NTP datagram from the server, with timeout.
	 * @return the datagram, or null if timed out.
	 */
	std::unique_ptr<ntp_datagram> recv();

	/** Request an update for a specific peer.
	 * This function will wait for a response up to the normal
	 * timeout, however if the response it delayed then it is
	 * possible for it to complete after this function has
	 * returned.
	 *
	 * The request is not retried following a timeout, in the
	 * expectation that the client will cycle through the
	 * other association IDs first (in order to give the
	 * maximum amount of time for any delayed response to be
	 * received).
	 * @param assoc_id the association ID for the peer
	 */
	void update_peer(uint16_t assoc_id);

	/** Request an update for the list of peers.
	 * This function will wait for a response up to the normal
	 * timeout, and repeat the request if the previous update
	 * found the server to be responsive.
	 *
	 * Regardless of whether the update is successful, all
	 * information from the previous update cycle will be
	 * deleted.
	 * @return true if successful, otherwise false
	 */
	bool update_peers();

	/** Build NTP attribute.
	 * @param builder an NTP attribute builder
	 */
	void build(ntp_attr_builder& builder);
};

} /* namespace horace */

#endif
