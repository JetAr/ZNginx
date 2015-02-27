# Makefile for ncftp
# 
# Major sections delimited by a dash lines.  If several lines set the same
# make variable, you can choose between the commented #samples, or just
# type what you want manually.
#--------------------------------------------------------------------------


# System dependent definitions.  See the README, part B.
#--------------------------------------------------------------------------
SDEFS = 


# Program definitions.  See the README, part C.
#--------------------------------------------------------------------------
PDEFS =
#PDEFS = -DGETLINE
#PDEFS = -DREADLINE -DCURSES
#PDEFS = -DSOCKS
#PDEFS = -DPASSIVEMODE
#PDEFS = -DDEBUG -DDB_ERRS


# Choose your compiler and flags below.  Make sure you use an ANSI compiler
# that handles new style function declarations and prototypes (gcc should).
#--------------------------------------------------------------------------
CC = cc
#CC = gcc

#CFLAGS = $(TERM_INC) -O
CFLAGS = $(TERM_INC) -O2
#CFLAGS = $(TERM_INC) -g

LFLAGS = -s
#LFLAGS =


# Additional libraries and/or object files.
# 
# For each library, add -lLIBNAME to the LIBS line below, for a library
#   named libLIBNAME.a.
#
# For each object file, just add the pathname of the object file.
#
# Some may need any of -lsocket, -lnet, -linet, -lintl, or -lnsl. 
# You'll need -lcurses or -ltermcap if CURSES is defined.
# You'll need -lreadline AND either -lcurses or -ltermcap if you
#   want to use the GNU Readline library.
# You'll need -lgetline (compile it as a library) if you want to use
#   getline.
# If your system is running Yellow Pages, you'll need to add the library
#   that has the YP/NIS version of getpwuid() in it (Important!)
# You'll need to know where the Rconnect.o object file is if you want
#   to use Socks.
#--------------------------------------------------------------------------
LIBS =
#LIBS = -ldbmalloc
#LIBS = -lgetline
#LIBS = -lreadline -lcurses
#LIBS = ../lib/Rconnect.o
#LIBS = -lnet -lnsl -lsocket -lcurses
#LIBS = -lcurses -ltermcap

# If the libraries are in a non-standard directory, or you if want to use
# getline or readline and they aren't installed system-wide, add the
# extra directories to look in here, using -L's.
#--------------------------------------------------------------------------
LIBDIRS =
#LIBDIRS = -L../getline
#LIBDIRS = -L../readline

# To make term sources define this to your term directory
TERM_INC = 
TERM_LIB = 
#TERM_INC = -include /usr/local/include/termnet.h
#TERM_LIB = -ltermnet

# Additional headers.
#
# If you defined READLINE or GETLINE, you have to tell where it's header
# file can be found.
#
# For READLINE, provide a path which would find <readline/readline.h>,
#   so you would put the parent directory of the readline directory below.
#   If you had '/usr/local/readline/readline.h' you would use
#   -I/usr/local.
#
# For GETLINE, a little different. Just supply a path that would find
#   <getline.h>.  If you had '/usr/local/getline/getline.h' you would use
#   -I/usr/local/getline.
#--------------------------------------------------------------------------
HDRDIRS =
#HDRDIRS = -I../getline
#HDRDIRS = -I..


# If you want to 'make install,' edit these variables, otherwise don't
# worry about it.
# To install MAN style pages, set MANDIR to the proper location.
# To install CATMAN style pages, set CATMANDIR, NROFF, and PACK to the proper
# locations.
# To inhibit the installation of either, unset MANDIR/CATMANDIR.
#--------------------------------------------------------------------------
#BINDIR = /usr/lbin
BINDIR = /usr/local/bin
MANDIR = /usr/man/man1
#MANDIR = 
#CATMANDIR = /usr/catman/LOCAL/g1
CATMANDIR =
NROFF	= /usr/ucb/nroff
PACK	= pack
TEST	= test
RM = rm -f
CP = cp
CAT = cat


#************************************************
#*** SHOULD NOT NEED TO EDIT BELOW THIS POINT ***
#************************************************

DEFS = $(PDEFS) $(SDEFS)
MK = $(CC) $(CFLAGS) $(DEFS) $(HDRDIRS) $(LFLAGS) $(LIBDIRS) $(LIBS)
 
SRCS = cmds.c cmdtab.c ftp.c ftprc.c getpass.c glob.c main.c open.c set.c \
tips.c util.c
  
HEADERS = cmds.h copyright.h defaults.h ftp.h ftprc.h getpass.h glob.h \
main.h open.h set.h sys.h util.h
   
OBJS = cmds.o cmdtab.o ftp.o ftprc.o getpass.o glob.o main.o open.o set.o \
tips.o util.o

NAME = ncftp
MAN = ncftp.1
CATMAN = ncftp.z
ALL = $(SRCS) $(HEADERS) patchlevel.h Blurb README Makefile $(MAN)

C_COMPILE = $(CC) $(CFLAGS) $(DEFS) $(HDRDIRS)
C_COMPILE2 = $(CC) $(CFLAGS) $(DEFS) -DMK='"$(MK)"' $(HDRDIRS)

all: $(NAME) done

$(NAME): $(OBJS)
	$(CC) $(LFLAGS) $(LIBDIRS) $(OBJS)  -o $(NAME) $(LIBS) $(TERM_LIB)

install: $(NAME)
	if $(TEST) -f $(BINDIR)/term ; then \
		$(CP) $(BINDIR)/term $(BINDIR)/$(NAME) ; \
		$(CAT) $(NAME) > $(BINDIR)/$(NAME) ; \
	else \
	  $(CP) $(NAME) $(BINDIR)/$(NAME) ; \
	fi
	@if $(TEST) -n '$(MANDIR)'; then \
		$(MAKE) install_man ; else true ;  fi
	@if $(TEST) -n '$(CATMANDIR)'; then \
		$(MAKE) install_catman ; else true ;  fi

install_man: $(MAN)
	$(CP) $(MAN)  $(MANDIR)/$(MAN)


install_catman:  $(CATMAN)
	$(CP) $(CATMAN)  $(CATMANDIR)/$(CATMAN)

uninstall:
	$(RM) $(BINDIR)/$(NAME)
	$(RM) $(MANDIR)/$(MAN)
	$(RM) $(CATMANDIR)/$(CATMAN)

$(CATMAN): $(MAN)
	$(RM) tmp
	$(NROFF) -man -Tlp $(MAN) > tmp
	$(PACK) -f tmp
	mv tmp.z $(CATMAN)

cmds.o:
	$(C_COMPILE2) cmds.c -c

.c.o:
	$(C_COMPILE) -c $<

done: $(NAME)
	-@ls -l $(NAME)
	-@echo 'Done.'

clean:
	rm -f $(OBJS) $(NAME)

# Dependencies:
cmds.o: cmds.c 
cmds.o: sys.h 
cmds.o: util.h 
cmds.o: cmds.h 
cmds.o: main.h 
cmds.o: ftp.h 
cmds.o: ftprc.h 
cmds.o: getpass.h 
cmds.o: glob.h 
cmds.o: open.h 
cmds.o: set.h 
cmds.o: defaults.h 
cmds.o: copyright.h 
cmdtab.o: cmdtab.c 
cmdtab.o: sys.h 
cmdtab.o: util.h 
cmdtab.o: cmds.h 
cmdtab.o: main.h 
cmdtab.o: ftp.h 
cmdtab.o: ftprc.h 
cmdtab.o: glob.h 
cmdtab.o: open.h 
cmdtab.o: set.h 
cmdtab.o: copyright.h 
ftp.o: ftp.c 
ftp.o: sys.h 
ftp.o: util.h 
ftp.o: ftp.h 
ftp.o: cmds.h 
ftp.o: main.h 
ftp.o: ftprc.h 
ftp.o: getpass.h 
ftp.o: defaults.h 
ftp.o: copyright.h 
ftprc.o: ftprc.c 
ftprc.o: sys.h 
ftprc.o: util.h 
ftprc.o: ftprc.h 
ftprc.o: main.h 
ftprc.o: cmds.h 
ftprc.o: set.h 
ftprc.o: defaults.h 
ftprc.o: copyright.h 
getpass.o: getpass.c 
getpass.o: sys.h 
getpass.o: util.h 
getpass.o: cmds.h 
getpass.o: getpass.h 
getpass.o: copyright.h 
glob.o: glob.c 
glob.o: sys.h 
glob.o: util.h 
glob.o: glob.h 
glob.o: cmds.h 
glob.o: copyright.h 
main.o: main.c 
main.o: sys.h 
main.o: util.h 
main.o: cmds.h 
main.o: main.h 
main.o: ftp.h 
main.o: ftprc.h 
main.o: open.h 
main.o: set.h 
main.o: defaults.h 
main.o: copyright.h 
open.o: open.c 
open.o: sys.h 
open.o: util.h 
open.o: open.h 
open.o: cmds.h 
open.o: ftp.h 
open.o: ftprc.h 
open.o: main.h 
open.o: defaults.h 
open.o: copyright.h 
set.o: set.c 
set.o: sys.h 
set.o: util.h 
set.o: cmds.h 
set.o: main.h 
set.o: set.h 
set.o: defaults.h 
set.o: copyright.h 
tips.o: tips.c 
tips.o: sys.h 
tips.o: util.h 
util.o: util.c 
util.o: sys.h 
util.o: util.h 
util.o: cmds.h 
util.o: main.h 
util.o: ftp.h 
util.o: ftprc.h 
util.o: defaults.h 
util.o: copyright.h 
