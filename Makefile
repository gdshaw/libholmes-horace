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
CXXFLAGS = -fPIC -O2 --std=c++17
LDLIBS = -ldl

SRC = $(wildcard src/*.cc)
BIN = $(SRC:src/%.cc=bin/%)
MAINBIN = bin/$(pkgname)
AUXBIN = $(filter-out $(MAINBIN),$(BIN))

HORACE = $(wildcard horace/*.cc)

EPDIRS = $(wildcard endpoints/*)
EPLIBS = $(foreach EPDIR,$(EPDIRS),$(EPDIR)/$(notdir $(EPDIR)).so)

.PHONY: all
all: $(BIN) $(EPLIBS)

$(BIN): bin/%: src/%.o horace.so
	@mkdir -p bin
	g++ -rdynamic -o $@ $^ $(LDLIBS)

endpoints/%.so: always
	make -C $(dir $@)

horace.so: $(HORACE:%.cc=%.o)
	gcc -shared -o $@ $^

.PHONY: clean
clean: $(EPDIRS:%=%/clean)
	rm -f horace/*.d horace/*.o
	rm -f src/*.d src/*.o
	rm -f *.a
	rm -rf bin

%/clean: always
	make -C $* clean

.PHONY: install
install: all
	@mkdir -p $(bindir)
	@mkdir -p $(libdir)
	@mkdir -p $(libexecdir)/$(pkgname)/bin
	@mkdir -p $(libexecdir)/$(pkgname)/endpoints
	cp $(MAINBIN) $(bindir)/
	cp horace.so $(libdir)/
	cp $(AUXBIN) $(libexecdir)/$(pkgname)/bin/
	cp $(EPLIBS) $(libexecdir)/$(pkgname)/endpoints/

.PHONY: uninstall
uninstall:
	rm -f $(bindir)/$(notdir $(MAINBIN))
	rm -rf $(libexecdir)/$(pkgname)

.PHONY: always
always:

-include $(HORACE:%.cc=%.d)
-include $(SRC:%.cc=%.d)
