GCC ?= gcc
CC = $(GCC)
LIBS = -lpthread
CFLAGS = -pthread -c -D__libHTTP__ -D_GNU_SOURCE -o $*.o
LFLAGS = -shared -rdynamic $(O) $(LIBS) -Wl,-soname,$(SO) -o $(SO)
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

O = strutils.o error.o connection.o method.o request.o response.o content.o init.o
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
