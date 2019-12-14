# LibHolmes-HORACE

## Purpose

LibHolmes is a library for the forensic analysis of files and network
traffic. This repository contains the network traffic capture, forwarding
and collection components of LibHolmes.

## Current status

This software is functional, however the protocol is not yet fully stable
and some important features are missing.

## Usage

Events can be captured using the horace capture command. For example,
the following command would capture network traffic from an interface
named 'ens3' and write it to a set of HORACE spoolfiles located in a
subdirectory of /var/spool/horace:

    horace capture netif:ens3 horace+file:/var/spool/horace

Captured data can then be forwarded to another location using the
horace forward command. For example, to forward the data captured above
to a host named horace.example.com:

    horace forward horace+file:/var/spool/horace \
        horace+tcp://horace.example.com

A HORACE server must be run on that host to receive the data. To do that,
writing the data into a local MongoDB database named 'horace':

    horace forward horace+tcp: mongodb://localhost/horace

Detailed documentation is provided in the form of manpages. See horace(1)
for a starting point. There is also a limited amount of online help which
can be obtained using the -h option with any HORACE command or subcommand:

    horace -h
    horace capture -h

## Limitations

The following configurations are not recommended:

- Capture directly to a TCP connection
- Reading or writing spoolfiles via a network filesystem

## Copyright and licensing

LibHolmes-HORACE is copyright 2019 Graham Shaw.

Redistribution and modification are permitted within the terms of the
BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

Please refer to the file [LICENCE.md](LICENCE.md) for details.

Note that the above licence applies only to the components of libholmes
published within this repository (libholmes-horace). The content of
the main libholmes repository is currently licensed using the GNU General
Public Licence (version 3 or any later version).

Third-party contributions are not currently being accepted into this
repository.
