#
# Licence: GPL
# Created: 2016-04-05 19:38:55+02:00
# Main authors:
#     - Jérôme Pouiller <jezz@sysmic.org>
#
CFLAGS += -Wall
PREFIX = /usr/local

all: computeSymetry debugPrint initAndPrint bestMasterShot bestPlayerShot paramMasterLevel libmastermind.so

paramMasterLevel: paramMasterLevel.o libmastermind.a
bestMasterShot: bestMasterShot.o libmastermind.a
bestPlayerShot: bestPlayerShot.o libmastermind.a
computeSymetry: computeSymetry.o libmastermind.a
debugPrint: debugPrint.o libmastermind.a
initAndPrint: initAndPrint.o libmastermind.a

libmastermind.a: mastermind.o
	$(AR) $(ARFLAGS) $@ $<

libmastermind.so: mastermind_shared.o
	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -shared -o $@

-include paramMasterLevel.d
-include bestMasterShot.d
-include bestPlayerShot.d
-include initAndPrint.d
-include debugPrint.d
-include computeSymetry.d
-include mastermind.d
-include mastermind_shared.d

%_shared.o: %.c
	$(COMPILE.c) -MMD -fPIC $(OUTPUT_OPTION) $<

%.o: %.c
	$(COMPILE.c) -MMD $(OUTPUT_OPTION) $<

clean:
	$(RM) *.o *.d

distclean: clean
	$(RM) compute_symetry init_and_print libmastermind.so libmastermind.a bestMasterShot bestPlayerShot paramMasterLevel

install:
	install -m 755 -d $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.a $(DESTDIR)$(PREFIX)/lib
	install -m 644 libmastermind.so $(DESTDIR)$(PREFIX)/lib


