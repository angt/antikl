CC     = cc
CFLAGS = -Wall -O2
prefix = /usr/local
PREFIX = $(prefix)

antikl:
	$(X)$(CC) $(EXTRA) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) antikl.c -o antikl

install: antikl
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	mv -f antikl $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/antikl

clean:
	rm -f antikl

.PHONY: antikl install uninstall clean
