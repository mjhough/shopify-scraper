CC=g++
CPPFLAGS_XML2 = $(shell xml2-config --cflags)
CPPFLAGS_CURL = $(shell curl-config --cflags)
CPPFLAGS=-Wall -g $(CPPFLAGS_CURL) $(CPPFLAGS_XML2)
LD=g++
LDLIBS_XML2 = $(shell xml2-config --libs)
LDLIBS_CURL = $(shell curl-config --libs)
LDLIBS= $(LDLIBS_XML2) $(LDLIBS_CURL) -lz -lcurl -lpthread # link with libz, libcurl, and pthreads
BIN=main

OBJS = main.o Producer.o Worker.o Consumer.o

all: main

main: $(OBJS)
	$(LD) $(CPPFLAGS) -o $@ $(OBJS) $(LDLIBS)

.c.o:
	$(CC) $(CPPFLAGS) -c $<

.PHONY: clean
clean:
	rm -f *.o *.out $(BIN) *.png *.html
