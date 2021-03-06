OS ?= unix

PREFIX ?= /usr/local

ifneq "$(MIRAGE_OS)" ""
OS := $(MIRAGE_OS)
endif

.PHONY: all clean install test
.DEFAULT: all

all:
	cd $(OS) && $(MAKE) all

clean:
	cd $(OS) && $(MAKE) clean

install:
	cd $(OS) && $(MAKE) install
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp scripts/mir-run $(DESTDIR)$(PREFIX)/bin/
	chmod a+x $(DESTDIR)$(PREFIX)/bin/mir-run

test:
	cd $(OS) && $(MAKE) test
