CC = gcc
LIBS = -lpthread
CFLAGS = -pthread -c -D__libHTTP__ -D_GNU_SOURCE -o $*.o
LFLAGS = -shared -rdynamic $(O) $(LIBS) -Wl,-soname,$(SO) -o $(SO)
AR = ar cr
RANLIB = ranlib
LN = ln -sfT
LIBDIR = /usr/lib/
PKGDIR = /usr/lib/pkgconfig/
INCDIR = /usr/include/libHTTP/

O = strutils.o error.o connection.o method.o request.o response.o content.o init.o
SO = libHTTP.so
A = libHTTP.a
LA = libHTTP.la
PC = libHTTP.pc

default: all

all: $(O) $(SO) $(LA) $(PC)

libHTTP.so:
	rm -f $(SO) $(A)
	$(AR) $(A) $(O)
	$(RANLIB) $(A)
	$(CC) $(LFLAGS)

install: all install-headers
	cp $(SO) $(LIBDIR)
	cp $(A) $(LIBDIR)
	cp $(LA) $(LIBDIR)
	cp $(PC) $(PKGDIR)
	chown root $(PKGDIR)$(PC)
	chgrp root $(PKGDIR)$(PC)
	chmod +r $(LIBDIR)$(LA)

install-headers:
	if [ ! -d $(INCDIR) ]; then \
		mkdir $(INCDIR); \
	fi
	rm -f $(INCDIR)*.h
	cp libHTTP.h $(INCDIR)
	chmod +r $(INCDIR)libHTTP.h

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f *.o *.a *.so *~

strutils.o: strutils.c
error.o: error.c
connection.o: connection.c
method.o: method.c
request.o: request.c
responce.o: response.c
content.o: content.c
init.o: init.c
