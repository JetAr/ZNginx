# Makefile for dynamically loadable modules
#
# Copyright (c) 1994-1997 Peter Eriksson <pen@signum.se>
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

LINKER	=	/usr/ccs/bin/ld -G -z text

MODULES = file.so dir.so cgi.so isproxy.so config.so \
	map.so ph.so ncsa-html.so mbox.so asis.so \
	http_proxy.so gopher_proxy.so ftp_proxy.so \
	auth_syspwd.so auth_file.so next_proxy.so \
#	auth_bis.so


.o.so:
	$(LINKER) -o $@ $<

.c.so:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<
	$(LINKER) -o $@ $*.o

.SUFFIXES:
.SUFFIXES: .so $(SUFFIXES)

fail:
	@echo 'NoNo! Don't run "make" in this directory!'
	@exit 1

clean:
	-rm -f *.o *.so *~ core \#*

all:	$(MODULES)

# Modules needing some special handling
map.so:	map.o
	$(LINKER) -o $*.so $< -lm



