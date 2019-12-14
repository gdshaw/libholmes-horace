# LibHolmes-HORACE

## Supported platforms

The majority of the code is intended to be portable to any POSIX-compatible
operating system, however this has not been tested, and the netif endpoint
is known to be Linux-specific due to the use of AF_PACKET sockets.

## Dependencies

This package has the following build-time dependencies.

- g++ (>= 7.4.0)
- make (>= 4.1)
- gzip (>= 1.6)
- pkg-config (>= 0.29.1)
- libsodium (>= 1.0.16)
- libmongoc (>= 1.9.2)
- libbson (>= 1.9.2)

Note that:

- The versions listed may in some cases be very conservative, however they
  are ones which have been tested and are known to work.
- A recent version of GCC is likely to be needed, as the code makes
  extensive use of C++11 and C++17 language features.

## Building and installing

A Makefile is supplied with support for the targets 'all', 'clean',
'install' and 'uninstall' with their customary meanings. Consequently, this
package may be built and installed using the following commands:

    make
    sudo make install
