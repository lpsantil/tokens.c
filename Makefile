# Copyright (c) 2020, Louis P. Santillan <lpsantil@gmail.com>
# All rights reserved.
# See LICENSE for licensing details.

DESTDIR ?= /usr/local

######################################################################
# Core count
CORES ?= 1

# Basic feature detection
OS ?= $(shell uname)
ARCH ?= $(shell uname -m)

######################################################################
CFLAGS ?= -Os -Wall -std=gnu99 -pedantic
LDFLAGS ?= -s

DDIR = docs
DSRC =
SRC = $(wildcard src/*.c)
OBJ = $(SRC:.c=.o)
SDEPS = $(SRC:.c=.d)
HDR =
IDIR = include
INC = $(IDIR)/$(HDR)
EDIR = .
EXE = tokens.exe
LNK =
LDIR = lib
LSRC = $(wildcard src/lib/*.c)
LOBJ = $(LSRC:.c=.o)
LSDEPS = $(LSRC:.c=.d)
LIB = $(LDIR)/lib$(LNK).a
TDIR = t
TSRC = $(wildcard t/*.c)
TOBJ = $(TSRC:.c=.o)
TSDEPS = $(TSRC:.c=.d)
TEXE = $(TOBJ:.o=.exe)

#TMPCI = $(shell cat tmp.ci.pid 2>/dev/null)
#TMPCT = $(shell cat tmp.ct.pid 2>/dev/null)
#TMPCD = $(shell cat tmp.cd.pid 2>/dev/null)

#CILOG ?= tmp.ci.log

######################################################################
######################## DO NOT MODIFY BELOW #########################
######################################################################

.PHONY: all test runtest clean install uninstall showconfig
.PHONY: gstat gpush help print-* tarball

%.o: %.c Makefile
	$(CC) $(CFLAGS) -MMD -MP -I$(IDIR) -c $< -o $@

t/%.exe: t/%.o $(LIB) Makefile
	$(LD) -L$(LDIR) -l$(LNK) $(LDFLAGS) $< -o $@

all: $(EXE)

$(LIB): $(LOBJ)
	$(AR) -rcs $@ $^

$(EXE): $(OBJ)
	$(LD) $^ $(LDFLAGS) -o $(EDIR)/$@

test: $(SYSINC) $(LIB) $(TEXE) Makefile

runtest: $(TEXE)
	for T in $^ ; do $(TAP) $$T ; done

clean:
	rm -f $(OBJ) $(EXE) $(LOBJ) $(LIB) $(TOBJ) $(TEXE) $(SYSINC) *.tmp $(SDEPS) $(LSDEPS) $(TSDEPS)

install: $(INC) $(LIB) $(EXE)
#	mkdir -p $(DESTDIR)/include/rt0 $(DESTDIR)/lib
#	rm -f .footprint
#	@for T in $(INC) $(LIB); \
#	do ( \
#		echo $(DESTDIR)/$$T >> .footprint; \
#		cp -v --parents $$T $(DESTDIR) \
#	); done

uninstall: .footprint
	@for T in `cat .footprint`; do rm -v $$T; done

-include $(SDEPS) $(LSDEPS) $(TSDEPS)

showconfig: p-OS p-ARCH p-DESTDIR p-CC p-LD p-AR p-CFLAGS p-LDFLAGS p-DDIR p-DSRC p-SRC p-OBJ p-HDR p-IDIR p-INC p-EDIR p-EXE p-TSRC p-TOBJ p-TEXE

gstat:
	git status

gpush:
	git commit
	git push

tarball:
	cd ../. && tar jcvf tokens.c.$(shell date +%Y%m%d%H%M%S).tar.bz2 tokens.c

define newline # a literal \n


endef
# Makefile debugging trick:
# call print-VARIABLE to see the runtime value of any variable
# (hardened a bit against some special characters appearing in the output)
p-%:
	@echo '$*=$(subst ','\'',$(subst $(newline),\n,$($*)))'
.PHONY: p-*

help: ## This help target
	@awk '/^[a-zA-Z0-9\-_+. ]*: ##/ { print; }' $(MAKEFILE_LIST)
