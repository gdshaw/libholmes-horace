# This file is part of libholmes.
# Copyright 2019 Graham Shaw
# Redistribution and modification are permitted within the terms of the
# BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
libexecdir = $(libdir)

pkgname = horace

CPPFLAGS = -MD -MP -I. '-DLIBEXECDIR="$(libexecdir)"' '-DPKGNAME="$(pkgname)"'
CXXFLAGS = -O2
LDLIBS = -ldl

SRC = $(wildcard src/*.cc)
BIN = $(SRC:src/%.cc=bin/%)

HORACE = $(wildcard horace/*.cc)

EPDIRS = $(wildcard endpoints/*)
EPLIBS = $(foreach EPDIR,$(EPDIRS),$(EPDIR)/$(notdir $(EPDIR)).so)

.PHONY: all
all: $(BIN) $(EPLIBS)

$(BIN): bin/%: src/%.o horace.a
	@mkdir -p bin
	g++ -rdynamic -o $@ $^ $(LDLIBS)

%.so: always
	make -C $(dir $@)

horace.a: $(HORACE:%.cc=%.o)
	ar rcs $@ $^

.PHONY: clean
clean: $(EPDIRS:%=%/clean)
	rm -f horace/*.d horace/*.o
	rm -f src/*.d src/*.o
	rm -f *.a
	rm -rf bin

%/clean: always
	make -C $* clean

.PHONY: always
always:

-include $(HORACE:%.cc=%.d)
-include $(SRC:%.cc=%.d)
