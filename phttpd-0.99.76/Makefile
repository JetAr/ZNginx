# Top level makefile for phttpd
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

##
## Where all files are to be installed/located
##
INSTROOT  = /opt/phttpd

##
## Which Make to use
##
MAKE = make

##
## Common C compiler flags for all compilers.
##
## Add "-DUNIXWARE" if compiling under UnixWare.
##
## Add "-DHAVE_VSNPRINTF" if your Unix have the __vsnprintf() function.
## (Solaris 2.5.1 does)
#
COMCFLAGS = -DINSTROOT='\"'$(INSTROOT)'\"' -DINCLUDE_ALLOC_STATS



##
## Uncomment this to link with the Electric Fence malloc debugger
## (an MT-Safe version is available by FTP from ftp.lysator.liu.se in
##  the directory pub/libraries)
##
# LIBEFENCE = -L/opt/gnu/lib -lefence


##
## Sun/Solaris - Gnu CC
##
## Add "-msupersparc" to CFLAGS if compiling for a SuperSPARC machine.
##
CC        = gcc -Wall
CFLAGS    = -g -O2 $(COMCFLAGS) -DUSE_GETDENTS -DUI_THREADS
LIBCFLAGS = -fpic
XOBJS     = solaris.o
LIBS      = -lthread -lnsl -lsocket -ldl 
MODLD	   = /usr/ccs/bin/ld -G -z text


##
## Sun/Solaris - SparcWorks C
##
#CC        = cc
#CFLAGS    = -g -O $(COMCFLAGS) -DUSE_GETDENTS -DUI_THREADS
#LIBCFLAGS = -Kpic
#XOBJS     = solaris.o
#LIBS      = -lthread -lnsl -lsocket -ldl
#MODLD	   = /usr/ccs/bin/ld -G -z text


##
## Sun/Solaris - Apogee C
##
#CC        = apcc
#CFLAGS    = -g -O $(COMCFLAGS) -DUSE_GETDENTS -DUI_THREADS
#LIBCFLAGS = -pic
#XOBJS 	   = solaris.o
#LIBS      = -lthread -lnsl -lsocket -ldl
#MODLD	   = /usr/ccs/bin/ld -G -z text


##
## UnixWare - Gnu CC
##
#CC        = gcc -Wall
#CFLAGS    = -g -O $(COMCFLAGS) -DUSE_GETDENTS -DUI_THREADS
#LIBCFLAGS = -fpic
#XOBJS	   =  unixware.o
#LIBS      = -lthread -lnsl -lsocket -ldl
#MODLD	   = /usr/ccs/bin/ld -G -z text

##
## Digital Unix 4.0 - cc
##
#CC      = cc -pthread
#CFLAGS  = -I../spilt -O $(COMCFLAGS) 
#LIBCFLAGS =
#XOBJS	= spilt_thread.o osf.o
#LIBS    = -lpthread
#MODLD	= ld -shared -expect_unresolved '*'
 
##
## IRIX 5.3 - pgcc
##
# CC	    = pgcc -Wall
# CFLAGS    = -I../spilt -O $(COMCFLAGS) -I/usr/local/pthreads/include -g  -D_SGI_MP_SOURCE -D_SGI_REENTRANT_FUNCTIONS 
# LIBCFLAGS = 
# XOBJS	    = spilt_thread.o irix.o hackpthreads.o
# LIBS      = -lpthread -ldl

##
## IRIX 6.2 - cc
##
# CC	    = cc
# CFLAGS    = -I../spilt -O $(COMCFLAGS) -D_SGI_MP_SOURCE -D_SGI_REENTRANT_FUNCTIONS -Wl,-woff,85
# LIBCFLAGS =
# XOBJS	    = spilt_thread.o irix.o hackpthreads.o
# LIBS      = -lpthread -ldl


INSTALL   = /usr/ucb/install
TAR       = tar


##### You should not have to modify anything below this line ###############


all:	phttpd modules ackpfd utils ptester dhttpd tests

install.all:	install install.conf install.etc 

install:	install.bin install.doc install.icons

install.srv:	phttpd
	@$(INSTALL) -d -m 0755 $(INSTROOT)/sbin
	$(INSTALL) -c -m 0755 src/phttpd/phttpd $(INSTROOT)/sbin

install.modules: modules
	@$(INSTALL) -d -m 0755 $(INSTROOT)/modules	
	$(INSTALL) -c -m 0755 src/modules/*.so $(INSTROOT)/modules

install.bin: install.srv install.modules ackpfd utils ptester dhttpd
	@$(INSTALL) -d -m 0755 $(INSTROOT)/bin
	$(INSTALL) -c -m 0755 src/ackpfd/ackpfd $(INSTROOT)/sbin
	$(INSTALL) -c -m 0755 src/dhttpd/dhttpd $(INSTROOT)/sbin
	$(INSTALL) -c -m 0755 src/ptester/ptester $(INSTROOT)/bin
	$(INSTALL) -c -m 0755 src/utils/logcvt-ip2n $(INSTROOT)/bin
	$(INSTALL) -c -m 0755 src/utils/htmlencode $(INSTROOT)/bin
	$(INSTALL) -c -m 0755 src/utils/httpdecode $(INSTROOT)/bin
	$(INSTALL) -c -m 0755 etc/rotate.logs $(INSTROOT)/bin
#	$(INSTALL) -c -m 0644 etc/purify.options $(INSTROOT)/bin/.purify
#	$(INSTALL) -c -m 0644 etc/purify.options $(INSTROOT)/sbin/.purify


install.conf:
	@$(INSTALL) -d -m 0755 $(INSTROOT)/modules
	@$(INSTALL) -d -m 0755 $(INSTROOT)/etc
	$(INSTALL) -m 0755 config/modules/*.conf $(INSTROOT)/modules
	$(INSTALL) -c -m 0644 config/phttpd.conf $(INSTROOT)/etc

install.doc:
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/phttpd
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/phttpd/doc
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/phttpd/doc/user
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/phttpd/doc/admin
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/phttpd/examples
	$(INSTALL) -c -m 0644 doc/phttpd/*.html $(INSTROOT)/db/phttpd
	$(INSTALL) -c -m 0644 doc/phttpd/*.gif $(INSTROOT)/db/phttpd
	$(INSTALL) -c -m 0644 examples/*.shtml $(INSTROOT)/db/phttpd/examples
	$(INSTALL) -c -m 0644 examples/*.map $(INSTROOT)/db/phttpd/examples
	$(INSTALL) -c -m 0644 examples/*.txt $(INSTROOT)/db/phttpd/examples
	$(INSTALL) -c -m 0644 examples/*.acl $(INSTROOT)/db/phttpd/examples
	$(INSTALL) -c -m 0755 examples/*.cgi $(INSTROOT)/db/phttpd/examples
	rm -fr $(INSTROOT)/db/phttpd/doc/admin
	cp -r doc/admin $(INSTROOT)/db/phttpd/doc
	rm -fr $(INSTROOT)/db/phttpd/doc/user
	cp -r doc/user $(INSTROOT)/db/phttpd/doc

install.icons:
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db
	@$(INSTALL) -d -m 0755 $(INSTROOT)/db/icons
	$(INSTALL) -c -m 0644 icons/* $(INSTROOT)/db/icons

install.etc:
	@if [ `id | awk '{print index($$1, "root")}'` -eq 0 ]; then \
		echo "Seeing as you aren't root I shall not install /etc/init.d/phttpd" ; \
	else \
		$(INSTALL) -c -m 0755 etc/phttpd.init /etc/init.d/phttpd ; \
	fi

#

phttpd:
	@(cd src/phttpd ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS) -D_REENTRANT -I.." LIBS="$(LIBS) $(LIBEFENCE)" XOBJS="$(XOBJS)")

modules:
	@(cd src/modules ; $(MAKE) all CC="$(CC)" CFLAGS="$(LIBCFLAGS) $(CFLAGS) -D_REENTRANT -I.." LINKER="$(MODLD)" LIBS="$(LIBS) $(LIBEFENCE)")

ackpfd:
	@(cd src/ackpfd ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS)" LIBS="$(LIBS) $(LIBEFENCE)")

utils:
	@(cd src/utils ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS)" LIBS="$(LIBS) $(LIBEFENCE)")

ptester:
	@(cd src/ptester ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS) -D_REENTRANT -I.." LIBS="$(LIBS) $(LIBEFENCE)")

dhttpd:
	@(cd src/dhttpd ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS) -D_REENTRANT -I.." LIBS="$(LIBS) $(LIBEFENCE)")

tests:
	@(cd src/tests ; $(MAKE) all CC="$(CC)" CFLAGS="$(CFLAGS) -D_REENTRANT -I.." LIBS="$(LIBS) $(LIBEFENCE)")

#


clean:
	@echo "Cleaning up..."
	@find . '(' -name '*~' -o -name 'core' -o -name '.nfs*' -o -name '%*' -o -name '#*' ')' -print -exec rm -f {} \;
	@(cd src/phttpd ; $(MAKE) clean)
	@(cd src/ackpfd ; $(MAKE) clean)
	@(cd src/modules ; $(MAKE) clean)
	@(cd src/tests ; $(MAKE) clean)
	@(cd src/utils ; $(MAKE) clean)
	@(cd src/ptester ; $(MAKE) clean)
	@(cd src/dhttpd ; $(MAKE) clean)

#

version:
	(PACKNAME=`basename \`pwd\`` ; echo 'char server_version[] = "'`echo $$PACKNAME | cut -d- -f2`'";' >src/phttpd/version.c)

#

dist:	clean version
	(PACKNAME=`basename \`pwd\`` ; cd .. ; $(TAR) cf - $$PACKNAME | gzip -9 >$$PACKNAME.tar.gz)

#
upload:	dist
	(PACKNAME=`basename \`pwd\`` ; scp ../$$PACKNAME.tar.gz ChangeLog pen@sparky.signum.se:/usr/local/ftp/pub/phttpd)

#
