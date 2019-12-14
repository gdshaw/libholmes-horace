# This file is part of libholmes.
# Copyright 2019 Graham Shaw
# Redistribution and modification are permitted within the terms of the
# BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

prefix = /usr/local
exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
libdir = $(exec_prefix)/lib
libexecdir = $(libdir)
mandir = $(prefix)/man
man1dir = $(mandir)/man1

pkgname = horace

CPPFLAGS = -MD -MP -I. '-DLIBEXECDIR="$(libexecdir)"' '-DPKGNAME="$(pkgname)"'
CXXFLAGS = -fPIC -O2 --std=c++17
LDLIBS = -ldl -pthread -lsodium

SRC = $(wildcard src/*.cc)
BIN = $(SRC:src/%.cc=bin/%)
MAINBIN = bin/$(pkgname)
AUXBIN = $(filter-out $(MAINBIN),$(BIN))

HORACE = $(wildcard horace/*.cc)

EPDIRS = $(wildcard endpoints/*)
EPLIBS = $(foreach EPDIR,$(EPDIRS),$(EPDIR)/$(notdir $(EPDIR)).so)

MAN1 = $(wildcard man/man1/*.1)
MAN1GZ = $(MAN1:man/%=man/%.gz)
MANGZ = $(MAN1GZ)

.PHONY: all
all: $(BIN) $(EPLIBS) $(MANGZ)

$(BIN): bin/%: src/%.o horace.so
	@mkdir -p bin
	g++ -rdynamic -o $@ $^ $(LDLIBS)

endpoints/%.so: always
	make -C $(dir $@)

horace.so: $(HORACE:%.cc=%.o)
	gcc -shared -o $@ $^

man/%.gz: man/%
	gzip -k -f $<

.PHONY: clean
clean: $(EPDIRS:%=%/clean)
	rm -f horace/*.d horace/*.o
	rm -f src/*.d src/*.o
	rm -f *.so
	rm -rf bin
	rm -f man/*/*.gz

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
	@mkdir -p $(man1dir)
	cp $(MAN1GZ) $(man1dir)/

.PHONY: uninstall
uninstall:
	rm -f $(bindir)/$(notdir $(MAINBIN))
	rm -rf $(libexecdir)/$(pkgname)
	rm -f $(foreach MANFILE,$(MAN1GZ),$(man1dir)/$(notdir $(MANFILE)))

.PHONY: always
always:

-include $(HORACE:%.cc=%.d)
-include $(SRC:%.cc=%.d)
