// This file is part of libholmes.
// Copyright 2020 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_NTP_DATAGRAM
#define LIBHOLMES_HORACE_NTP_DATAGRAM

#include <string>

namespace horace {

/** A class to represent an NTP mode 6 datagram. */
class ntp_datagram {
private:
	/** The raw content of the datagram.
	 * This must be at least 12 octets long.
	 */
	std::basic_string<unsigned char> _content;
public:
	/** Construct NTP datagram from raw content.
	 * @param content the raw content
	 */
	ntp_datagram(std::basic_string<unsigned char> content);

	/** Construct NTP datagram for given parameters.
	 * @param opcode the required opcode
	 * @param seqnum the sequence number
	 * @param assoc_id the required association ID
	 */
	ntp_datagram(unsigned int opcode, uint16_t seqnum, uint16_t assoc_id);

	/** Get the raw content of this datagram.
	 * @return the raw content
	 */
	const std::basic_string<unsigned char>& content() const {
		return _content;
	}

	/** Get the payload of this datagram.
	 * @return the payload
	 */
	std::basic_string<unsigned char> payload() const {
		return _content.substr(12, count());
	}

	/** Check whether this is a response datagram.
	 * @return true if a response, false if a request
	 */
	bool is_response() const {
		return _content[1] & 0x80;
	}

	/** Check whether this datagram reports an error.
	 * @return true if reporting an error, otherwise false
	 */
	bool is_error() const {
		return _content[1] & 0x40;
	}

	/** Check whether there are more datagrams to follow.
	 * @return true if more to follow, otherwise false
	 */
	bool more() const {
		return _content[1] & 0x20;
	}

	/** Get the opcode.
	 * @return the opcode
	 */
	unsigned int opcode() const {
		return _content[1] & 0x1f;
	}

	/** Get the sequence number.
	 * @return the sequence number
	 */
	uint16_t seqnum() const {
		uint16_t seqnum = _content[2];
		seqnum <<= 8;
		seqnum |= _content[3];
		return seqnum;
	}

	/** Get the status code.
	 * @return the status code
	 */
	uint16_t status() const {
		uint16_t status = _content[4];
		status <<= 8;
		status |= _content[5];
		return status;
	}

	/** Get the association ID.
	 * @return the association ID
	 */
	uint16_t assoc_id() const {
		uint16_t assoc_id = _content[6];
		assoc_id <<= 8;
		assoc_id |= _content[7];
		return assoc_id;
	}

	/** Get the offset.
	 * @return the offset, in octets
	 */
	size_t offset() const {
		size_t offset = _content[8];
		offset <<= 8;
		offset |= _content[9];
		return offset;
	}

	/** Get the length of the payload.
	 * @return the length, in octets
	 */
	size_t count() const {
		size_t count = _content[10];
		count <<= 8;
		count |= _content[11];
		return count;
	}

	/** Check whether this datagram responds to a given request
	 * @param request the request against which to compare
	 * @return true if this is a valid response, otherwise false
	 */
	bool responds_to(const ntp_datagram& request);
};

} /* namespace horace */

#endif
