// This file is part of libholmes.
// Copyright 2019 Graham Shaw
// Redistribution and modification are permitted within the terms of the
// BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

#ifndef LIBHOLMES_HORACE_ADDRESS_FILTER
#define LIBHOLMES_HORACE_ADDRESS_FILTER

#include <memory>
#include <list>

#include "horace/inet4_netblock.h"
#include "horace/filter.h"

namespace horace {

/** A filter class for filtering out a list of IP netblocks.
 * Currently only IPv4 netblocks are supported. Frames are filtered out if
 * they contain an IPv4 datagram with either a source or destination address
 * which matches one of the supplied netblocks. All other traffic is allowed
 * to pass, including frames which do not contain IPv4 datagrams.
 */
class address_filter:
	public filter {
private:
	/** The list of IPv4 netblocks to exclude. */
	std::list<inet4_netblock> _inet4_netblocks;

	/** True if the address list has been modified since the filter
	 * was last compiled, otherwise false.
	 */
	mutable bool _modified;

	/** The list of compiled instructions. */
	mutable std::unique_ptr<sock_filter[]> _filter;

	/** The compiled filter. */
	mutable sock_fprog _fprog;

	/** Unconditionally compile this filter.
	 * The compiled filter is written to _fprog.
	 */
	void _compile() const;
public:
	/** Construct empty address filter. */
	address_filter();

	address_filter(const address_filter&) = delete;
	address_filter& operator=(const address_filter&) = delete;

	/** Add an IPv4 netblock to this filter.
	 * @param nb the netblock to be added
	 */
	void add(const inet4_netblock& nb);

	virtual bool empty() const;
	virtual const struct sock_fprog* compile() const;
};

} /* namespace horace */

#endif
