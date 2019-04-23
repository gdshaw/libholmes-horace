# This file is part of libholmes.
# Copyright 2019 Graham Shaw
# Redistribution and modification are permitted within the terms of the
# BSD-3-Clause licence as defined by v3.4 of the SPDX Licence List.

CPPFLAGS = -MD -MP -I.
CXXFLAGS = -O2

HORACE = $(wildcard horace/*.cc)

.PHONY: all
all: horace.a

horace.a: $(HORACE:%.cc=%.o)
	ar rcs $@ $^

.PHONY: clean
clean:
	rm -f horace/*.d horace/*.o
	rm -f *.a

-include $(HORACE:%.cc=%.d)
