CC=gcc
CFLAGS=-g -Wall

SRCDIR=src
INCDIR=include
LIBDIR=lib

OBJS = utils.o hash.o sha256.o
LOBJS = $(addprefix $(LIBDIR)/, $(OBJS))

# Make target to build for final submission
all: merkle child_process

# Make target to build for testing intermediate submission
inter: CFLAGS += -D TEST_INTERMEDIATE
inter: merkle

merkle: $(SRCDIR)/merkle.c $(LIBDIR)/utils.o $(LIBDIR)/print_tree.o
	$(CC) $(CFLAGS) -I$(INCDIR) $(SRCDIR)/merkle.c $(LIBDIR)/utils.o $(LIBDIR)/print_tree.o -o merkle

child_process: $(SRCDIR)/child_process.c $(LOBJS) 
	$(CC) $(CFLAGS) -I$(INCDIR) $(SRCDIR)/child_process.c $(LOBJS) -o child_process

${LIBDIR}/utils.o: $(SRCDIR)/utils.c $(INCDIR)/utils.h
	$(CC) $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/utils.c -o $(LIBDIR)/utils.o

t1:
	./merkle input/T1.txt 8

t2:
	./merkle input/T2.txt 16

t3:
	./merkle input/T3.txt 32

.PHONY: clean all t1 t2 t3

clean:
	rm -f merkle child_process lib/utils.o
	rm -rf output/ *.dSYM/