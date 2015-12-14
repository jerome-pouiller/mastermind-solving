#
# Licence: GPL
# Created: 2015-12-14 21:36:12+01:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall
PREFIX = /usr/local

all: mastermind

mastermind: mastermind.o
-include mastermind.d


%.o: %.c
	$(COMPILE.c) -MMD -c -o $@ $<

clean:
	rm -f *.o *.d

distclean: clean
	rm -f mastermind

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 mastermind $(DESTDIR)$(PREFIX)/bin


