# Makefile for phttpd
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

XOBJS =

OBJS = 	main.o http.o mime.o url.o config.o \
	html.o version.o error.o modules.o logger.o signals.o \
	fdstdio.o table.o clist.o process.o \
	strmatch.o atotm.o safeio.o access.o \
	hashtable.o fscache.o urlcache.o cache.o strutils.o \
	hostcache.o usercache.o readdir.o \
	$(XOBJS)

fail:
	@echo 'NoNo! Don't run "make" in this directory!'
	@exit 1

all:	phttpd


$(OBJS): ../phttpd.h

phttpd: $(OBJS)
	$(CC) -o phttpd $(OBJS) $(LIBS)


clean:
	-rm -f *.o phttpd



