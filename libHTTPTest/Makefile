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
LIBS = $(shell pkg-config --libs libHTTP)
CFLAGS_EXTRA = $(shell pkg-config --cflags libHTTP)
CFLAGS = -c $(CFLAGS_EXTRA) -o $*.o
LFLAGS = $(O) $(LIBS) -o $(EXE)
CHMOD = chmod 0755
INSTALL = install -m755
INSTALL_DIR = $(INSTALL) -d

O = main.o getch.o
EXE = libHTTPTest.out

default: all

all: $(EXE)

libHTTPTest.out: $(O)
	$(CC) $(LFLAGS)
	$(CHMOD) $(EXE)

install: all
	$(INSTALL_DIR) ../bin
	$(INSTALL) $(EXE) ../bin/libHTTPTest

.c.o:
	$(CC) $(CFLAGS) $*.c

clean:
	rm -f *.o *.out *~

.PHONY: clean .c.o install all default

main.o: main.c
getch.o: getch.c
