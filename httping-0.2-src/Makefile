#### Uncomment one of the following:
## Visual C++
CC = cl
CFLAGS = -GX -Os -G5
LFLAGS = wsock32.lib /Fe$@
OBJEXT = obj

## Borland C++
#CC = bcc32
#CFLAGS = -Os -5
#LFLAGS = -e$@
#OBJEXT = obj

## Cygwin/GNU C++
#CC = g++
#CFLAGS = -Wall -O
#LFLAGS = -lwsock32 -o $@
#OBJEXT = o


.SUFFIXES: .cpp .$(OBJEXT)

## Build rule for VC++ and BC++
.cpp.$(OBJEXT):
	$(CC) -c $(CFLAGS) $<

## Build rule for Cygwin/GNU C++
#.cpp.$(OBJEXT):
#	$(CC) -c $(CFLAGS) $< -o $*.o


#### Major targets #####################################################

all: httping.exe
ctags:
	ctags *.cpp *.h

clean:
	del *.$(OBJEXT) *.bak

distclean: clean
	del *.exe *.swp


#### Common dependencies ###############################################

main.$(OBJEXT): main.cpp
ws-util.$(OBJEXT): ws-util.cpp ws-util.h


#### tcping ##############################################

HTTPING_OBJS = httping.$(OBJEXT) main.$(OBJEXT) \
		ws-util.$(OBJEXT)
httping.exe: $(HTTPING_OBJS)
	$(CC) $(HTTPING_OBJS) $(LFLAGS)

httping.$(OBJEXT): httping.cpp ws-util.h


