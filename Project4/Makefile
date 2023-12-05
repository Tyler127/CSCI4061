CC=gcc
CFLAGS=-g -Wall -pthread

SRCDIR=src
INCDIR=include
LIBDIR=lib

all: outdir $(LIBDIR)/utils.o server client

server: $(LIBDIR)/utils.o $(INCDIR)/utils.h $(SRCDIR)/server.c
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(LIBDIR)/utils.o $(SRCDIR)/server.c -lm

client: $(LIBDIR)/utils.o $(INCDIR)/utils.h $(SRCDIR)/client.c
	$(CC) $(CFLAGS) -I$(INCDIR) -o $@ $(LIBDIR)/utils.o $(SRCDIR)/client.c -lm
	
.PHONY: clean outdir

clean:
	rm -f server client
	rm -rf output

test: clean all
	./server & 
	for i in `seq 0 10`; do ./client "img/$$i" "output/$$i" 180; done; 
	pkill -f ./server

outdir: 
	for i in `seq 0 10`; do mkdir -p -m 0777 "output/$$i"; done;


