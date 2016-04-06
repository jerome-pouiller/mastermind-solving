#
# Licence: GPL
# Created: 2016-04-05 19:38:55+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall
PREFIX = /usr/local

all: compute_symetry init_and_print libmastermind.so

compute_symetry: compute_symetry.o libmastermind.a
init_and_print: init_and_print.o libmastermind.a

libmastermind.a: mastermind.o
	$(AR) $(ARFLAGS) $@ $<

libmastermind.so: LDFLAGS=-shared
libmastermind.so: mastermind_shared.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

-include init_and_print.d
-include compute_symetry.d
-include mastermind.d
-include mastermind_shared.d

%_shared.o: %.c
	$(COMPILE.c) -MMD -fPIC $(OUTPUT_OPTION) $<

%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

clean:
	$(RM) *.o *.d

distclean: clean
	$(RM) compute_symetry init_and_print libmastermind.so libmastermind.a

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.a $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.so $(DESTDIR)$(PREFIX)/lib
	install -m 755 -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 compute_symetry $(DESTDIR)$(PREFIX)/bin
	install -m 755 init_and_print $(DESTDIR)$(PREFIX)/bin


