#
# $Id: Makefile,v 2.6 2005/10/20 11:24:12 martin Exp $
# Copyright (c) 2004, 2005, Voidsoft AB
#

# Version
MAJOR	= 1
MINOR	= 2

PREFIX = /usr/local

CC	= g++
CFLAGS	= -Wall -ansi -fPIC
STRIPFLAG  = -s

ifdef DEBUG
CFLAGS    += -DDEBUG -g
STRIPFLAG  =  
endif

OBJS	= tdspp.o \
	  query.o \
	  field.o

INCS	= -I/usr/local/include

LIBS 	= -L/usr/local/lib -lct -shared -ldl

LIBNAME = tds++
LIB 	= libtds++.so.$(MAJOR).$(MINOR)
SONAME  = libtds++.so

.cc.o:	*.hh
	$(CC) -o $@ $(INCS) -c $(CFLAGS) $<

$(LIB): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $(INCS) $(LIBS) $(OBJS) $(LDFLAGS)

all:: $(LIB)

main:: 
	g++ -g -ltds++ main.cc -o main
clean::
	rm -f $(OBJS)
	rm -f $(LIB)

install:: $(LIB) 
	@ls -l $(LIB)
	@echo "Installing $(SONAME) into $(PREFIX)"
	@mkdir -p $(PREFIX)/lib/
	@mkdir -p $(PREFIX)/include/$(LIBNAME)
	install -o root -g bin -m 644 $(STRIPFLAG) $(LIB) $(PREFIX)/lib/
	@ln -fs $(PREFIX)/lib/$(LIB) $(PREFIX)/lib/$(SONAME)
	install -o root -g bin -m 644 *.hh $(PREFIX)/include/$(LIBNAME)

