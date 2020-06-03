// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_RESPONSE
#define LIBHOLMES_HORACE_NTP_RESPONSE

#include <memory>
#include <map>

#include "ntp_datagram.h"

namespace horace {

/** A class to represent a (possibly incomplete) response to an NTP request.
 * The response may be composed of multiple datagrams.
 */
class ntp_response {
private:
	/** The fragments, indexed by offset. */
	std::map<uint16_t, std::unique_ptr<ntp_datagram>> _fragments;
public:
	/** Add a newly-received fragment to this response.
	 * @param fragment the fragment to be added
	 */
	void add(std::unique_ptr<ntp_datagram>& fragment);

	/** Check whether the response is complete.
	 * @return true is complete, otherwise false
	 */
	bool complete() const;

	/** Get the payload.
	 * @return the payload
	 */
	std::basic_string<unsigned char> payload() const;

	/** Get the payload, as a character string.
	 * @return the payload
	 */
	std::string text_payload() const;

	/** Get the association ID.
	 * This is taken from the first fragment.
	 * @return the association ID
	 */
	uint16_t associd() const;

	/** Get the status code.
	 * This is taken from the first fragment.
	 * @return the status code
	 */
	uint16_t status() const;
};

} /* namespace horace */

#endif
