#
# Licence: GPL
# Created: 2016-04-05 19:38:55+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall -I . -g3 -O3 -DNDEBUG
PREFIX = /usr/local

all: tests/computeSymetry tests/debugPrint tests/initAndPrint \
     tests/bestPlayerNextTwoShots tests/bestMasterShot tests/bestPlayerShot \
     tests/bestPlayerEnd tests/paramMasterLevel tests/obvious libmastermind.so

tests/paramMasterLevel: tests/paramMasterLevel.o libmastermind.a
tests/bestMasterShot: tests/bestMasterShot.o libmastermind.a
tests/bestPlayerNextTwoShots: tests/bestPlayerNextTwoShots.o libmastermind.a
tests/bestPlayerShot: tests/bestPlayerShot.o libmastermind.a
tests/bestPlayerEnd: tests/bestPlayerEnd.o libmastermind.a
tests/computeSymetry: tests/computeSymetry.o libmastermind.a
tests/debugPrint: tests/debugPrint.o libmastermind.a
tests/initAndPrint: tests/initAndPrint.o libmastermind.a
tests/obvious: tests/obvious.o libmastermind.a

libmastermind.a: mastermind.o
	$(AR) $(ARFLAGS) $@ $<

libmastermind.so: mastermind_shared.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -shared -o $@

-include tests/paramMasterLevel.d
-include tests/bestMasterShot.d
-include tests/bestPlayerNextTwoShots.d
-include tests/bestPlayerShot.d
-include tests/bestPlayerEnd.d
-include tests/initAndPrint.d
-include tests/debugPrint.d
-include tests/computeSymetry.d
-include tests/mastermind.d
-include tests/mastermind_shared.d
-include tests/obvious.d

%_shared.o: %.c
	$(COMPILE.c) -MMD -fPIC $(OUTPUT_OPTION) $<

%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

clean:
	$(RM) *.o *.d tests/*.o tests/*.d

distclean: clean
	$(RM) tests/initAndPrint tests/debugPrint tests/computeSymetry \
	      tests/bestPlayerEnd tests/bestPlayerShot tests/bestPlayerNextTwoShots \
	      tests/bestMasterShot tests/paramMasterLevel tests/obvious \
	      libmastermind.so libmastermind.a

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.a $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.so $(DESTDIR)$(PREFIX)/lib


