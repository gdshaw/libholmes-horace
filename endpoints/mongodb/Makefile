# This file is part of libholmes.
# Copyright 2019 Graham Shaw
# Redistribution and modification are permitted within the terms of the
# BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

ENDPOINT := $(notdir $(shell pwd))

CPPFLAGS = -MD -MP -I../.. `pkg-config --cflags libmongoc-1.0`
CXXFLAGS = -fPIC -O2 --std=c++17
LDLIBS = `pkg-config --libs libmongoc-1.0`

SRC = $(wildcard *.cc)

$(ENDPOINT).so: $(SRC:%.cc=%.o)
	gcc -shared -o $@ $^ $(LDLIBS)

clean:
	rm -f *.d *.o *.so

-include $(SRC:%.cc=%.d)
