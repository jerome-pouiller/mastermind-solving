#
# Licence: GPL
# Created: 2016-04-05 19:38:55+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall
PREFIX = /usr/local

all: tests init_and_print

# tests: LDLIBS += -lm
tests: tests.o mastermind.o
init_and_print: init_and_print.o mastermind.o
-include init_and_print.d
-include tests.d
-include mastermind.d


%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

clean:
	$(RM) *.o *.d

distclean: clean
	$(RM) tests init_and_print

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 tests $(DESTDIR)$(PREFIX)/bin
	install -m 755 init_and_print $(DESTDIR)$(PREFIX)/bin


