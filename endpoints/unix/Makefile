# This file is part of libholmes.
# Copyright 2019 Graham Shaw
# Redistribution and modification are permitted within the terms of the
# BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

ENDPOINT := $(notdir $(shell pwd))

CPPFLAGS = -MD -MP -I../.. -idirafter ../../compat
CXXFLAGS = -fPIC -O2 --std=c++17

SRC = $(wildcard *.cc)

$(ENDPOINT).so: $(SRC:%.cc=%.o)
	gcc -shared -o $@ $^

clean:
	rm -f *.d *.o *.so

-include $(SRC:%.cc=%.d)
