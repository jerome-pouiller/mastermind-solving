#
# Licence: GPL
# Created: 2016-04-05 19:38:55+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall -I . -g3 -O3 -DNDEBUG
PREFIX = /usr/local

all: tests/bestMasterShot tests/bestPlayerEnd tests/bestPlayerNextTwoShots \
     tests/bestPlayerShot tests/computeSymetry tests/debugPrint \
     tests/initAndPrint tests/obvious tests/paramMasterLevel libmastermind.so

tests/bestMasterShot: tests/bestMasterShot.o libmastermind.a
tests/bestPlayerEnd: tests/bestPlayerEnd.o libmastermind.a
tests/bestPlayerNextTwoShots: tests/bestPlayerNextTwoShots.o libmastermind.a
tests/bestPlayerShot: tests/bestPlayerShot.o libmastermind.a
tests/computeSymetry: tests/computeSymetry.o libmastermind.a
tests/debugPrint: tests/debugPrint.o libmastermind.a
tests/initAndPrint: tests/initAndPrint.o libmastermind.a
tests/obvious: tests/obvious.o libmastermind.a
tests/paramMasterLevel: tests/paramMasterLevel.o libmastermind.a

libmastermind.a: mastermind.o
	$(AR) $(ARFLAGS) $@ $<

libmastermind.so: mastermind_shared.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -shared -o $@

include $(wildcard tests/*.d)
include $(wildcard *.d)

%_shared.o: %.c
	$(COMPILE.c) -MMD -fPIC $(OUTPUT_OPTION) $<

%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

clean:
	$(RM) *.o *.d tests/*.o tests/*.d

distclean: clean
	$(RM) tests/bestMasterShot tests/bestPlayerEnd tests/bestPlayerNextTwoShots \
	      tests/bestPlayerShot tests/computeSymetry tests/debugPrint \
	      tests/initAndPrint tests/obvious tests/paramMasterLevel \
	      libmastermind.so libmastermind.a

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.a $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.so $(DESTDIR)$(PREFIX)/lib


