CC=gcc
CFLAGS=-O3 -Wunused-result
HTTPOBJ = httppil.o http.o httpxml.o httphandler.o httppost.o httpauth.o
HEADERS = httpint.h httpapi.h httpxml.h
ifndef TARGET
TARGET = miniweb
HTTPOBJ += miniweb.o
endif

DEFINES=

ifdef WINDIR
DEFINES+= -DWIN32
LDFLAGS += -lwsock32
OS="Win32"
else
#CFLAGS+= -fPIC
OS="Linux"
endif

ifndef DEBUG
DFLAGS += -s
else
DEFINES+= -DHTTP_DEBUG
LDFLAGS += -g
endif

ifdef ENABLE_SERIAL
HTTPOBJ+= httpserial.o libctb/src/fifo.o libctb/src/serportx.o
HEADERS+= httpserial.h
DEFINES+= -Ilibctb/include
endif

%.o: %.c $(HEADERS)
	$(CC) $(DEFINES) -c -o $@ $(CFLAGS) $(filter %.c, $^)

all: $(HTTPOBJ)
	@echo Building for $(OS)
	$(CC) $(LDFLAGS) $(HTTPOBJ) -o $(TARGET)

min: $(HTTPOBJ) httpmin.o
	@echo Building for $(OS)
	$(CC) $(LDFLAGS) $(HTTPOBJ) httpmin.o -o httpd

install: all
	@rm -f /usr/bin/$(TARGET)
	@cp $(TARGET) /usr/bin

clean:
	@rm -f $(TARGET) $(TARGET).exe
	@rm -f *.o
	@rm -rf Debug Release
