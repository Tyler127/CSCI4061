CC=gcc
CFLAGS=-g -Wall

SRCDIR=src
INCDIR=include
LIBDIR=lib

OBJS = hash.o sha256.o utils.o
LOBJS = $(addprefix $(LIBDIR)/, $(OBJS))

# Make target to build for final submission
all: root_process leaf_process nonleaf_process

root_process: $(SRCDIR)/root_process.c $(LIBDIR)/utils.o
	$(CC) $(CFLAGS) -I$(INCDIR) $(SRCDIR)/root_process.c $(LIBDIR)/utils.o -o root_process
	
leaf_process: $(SRCDIR)/leaf_process.c $(LOBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) $(SRCDIR)/leaf_process.c $(LOBJS) -o leaf_process
	
nonleaf_process: $(SRCDIR)/nonleaf_process.c $(LOBJS)
	$(CC) $(CFLAGS) -I$(INCDIR) $(SRCDIR)/nonleaf_process.c $(LOBJS) -o nonleaf_process

${LIBDIR}/utils.o: $(SRCDIR)/utils.c $(INCDIR)/utils.h
	$(CC) $(CFLAGS) -I$(INCDIR) -c $(SRCDIR)/utils.c -o $(LIBDIR)/utils.o

ROOT_DIRS = root1 root2 root3
SOURCE_PREFIX = original_

# Helper function to prepare a root directory
define prepare_root_dir

	find . -name '.DS_Store' -type f -delete
	rm -rf ./root_directories/$(1)
	cp -r ./root_directories/$(SOURCE_PREFIX)$(1) ./root_directories/$(1)

endef

inter:
	$(foreach root, $(ROOT_DIRS), $(call prepare_root_dir,$(root)))
	./leaf_process ./root_directories/root1/sub_1/WorldStage_2.txt 0
	./leaf_process ./root_directories/root2/FairySong_1.txt 0
	./leaf_process ./root_directories/root3/sub_4/subsub_4-1/Madrigal_4.txt 0

root1 root2 root3:
	$(call prepare_root_dir,$@)
	./root_process ./root_directories/$@

final:
	$(foreach root, $(ROOT_DIRS), $(call prepare_root_dir,$(root)))
	$(foreach root, $(ROOT_DIRS), ./root_process ./root_directories/$(root);)

.PHONY: inter root1 root2 root3 final clean all

clean:
	rm -f root_process leaf_process nonleaf_process
	rm -rf *.dSYM/
