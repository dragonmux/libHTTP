# This file is part of libHTTP
# Copyright © 2013 Rachel Mant (dx-mon@users.sourceforge.net)
#
# libHTTP is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libHTTP is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

GCC ?= gcc
CC = $(GCC)
LIBS = -lpthread
CFLAGS = -pthread -c -D__libHTTP__ -D_GNU_SOURCE -o $*.o
LFLAGS = -shared -rdynamic $(O) $(LIBS) -Wl,-soname,$(SO) -o $(SO) -z defs
AR = ar cr
RANLIB = ranlib
LN = ln -sfT
PREFIX ?= /usr
LIBDIR ?= $(PREFIX)/lib
PKGDIR = $(LIBDIR)/pkgconfig
INCDIR = $(PREFIX)/include/libHTTP
INSTALL = install
INSTALL_FILE = $(INSTALL) -m644
INSTALL_DIR = $(INSTALL) -m755 -d

O = strutils.o error.o connection.o method.o request.o response.o content.o internals.o init.o
SO = libHTTP.so
A = libHTTP.a
PC = libHTTP.pc
IN = libHTTP.pc.in

default: all

all: $(SO) $(PC)

libHTTP.so: $(O)
	$(AR) $(A) $(O)
	$(RANLIB) $(A)
	$(CC) $(LFLAGS)

$(LIBDIR):
	$(INSTALL_DIR) $(LIBDIR)

$(PKGDIR):
	$(INSTALL_DIR) $(PKGDIR)

$(INCDIR):
	$(INSTALL_DIR) $(INCDIR)

install: all $(LIBDIR) $(PKGDIR) install-headers
	$(INSTALL_FILE) $(SO) $(LIBDIR)
	$(INSTALL_FILE) $(A) $(LIBDIR)
	$(INSTALL_FILE) $(PC) $(PKGDIR)

install-headers: $(INCDIR)
	$(INSTALL_FILE) libHTTP.h $(INCDIR)

libHTTP.pc:
	$(SHELL) -c "sed -e 's:@LIBDIR@:$(LIBDIR):g' -e 's:@PREFIX@:$(PREFIX):g' $(IN) > $(PC)"

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f *.o *.a *.so *~

.PHONY: clean .c.o libHTTP.pc install-headers install all default

strutils.o: strutils.c
error.o: error.c
connection.o: connection.c
method.o: method.c
request.o: request.c
responce.o: response.c
content.o: content.c
init.o: init.c
internals.o: internals.c
