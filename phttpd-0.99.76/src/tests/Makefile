# Makefile for the test programs
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

TPROGS = tester test.so test_pgm test_form test_post

#

fail:
	@echo 'NoNo! Don't run "make" in this directory!'
	@exit 1
	
all:	$(TPROGS)

#

tester:		tester.o
	$(CC) -o tester tester.o -lsocket -lnsl -ldl

test.so:	test_so.o
	/usr/ccs/bin/ld -G -o test.so test_so.o

test_pgm:	test_pgm.c
	$(CC) -o test_pgm test_pgm.c

test_form:	test_form.c
	$(CC) -o test_form test_form.c

test_post:	test_post.c
	$(CC) -o test_post test_post.c

#

clean:
	-rm -f core *.o $(TPROGS)

