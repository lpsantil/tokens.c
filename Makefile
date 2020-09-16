# Copyright (c) 2017, Louis P. Santillan <lpsantil@gmail.com>
# All rights reserved.
# See LICENSE for licensing details.

DESTDIR ?= /usr/local

######################################################################
# Core count
CORES ?= 1

# Basic feature detection
#OS = $(shell ( cat /etc/os-release | grep "rhel\|fedora\|centos" && echo "rhel" ) || cat /etc/os-release | grep "debian\|ubuntu" && echo "debian" || uname)
OS ?= $(shell uname)
ARCH ?= $(shell uname -m)

######################################################################
CFLAGS ?= -Os -Wall -std=gnu99 -pedantic
LDFLAGS ?= -s
#LDFLAGS ?= -s -nostdlib -Wl,--gc-sections

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

TMPCI = $(shell cat tmp.ci.pid 2>/dev/null)
TMPCT = $(shell cat tmp.ct.pid 2>/dev/null)
TMPCD = $(shell cat tmp.cd.pid 2>/dev/null)

CILOG ?= tmp.ci.log

# DEPS
DEPS =
LIBDEP =

# TDEPS
TDEPS =
TAP =
LIBTAP =

######################################################################
######################## DO NOT MODIFY BELOW #########################
######################################################################

.PHONY: all test runtest clean start_ci stop_ci start_ct stop_ct
.PHONY: start_cd stop_cd install uninstall showconfig gstat gpush
.PHONY: help print-* tarball

#%.o: %.c $(INC) Makefile
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

#start_ci:
#	( watch time -p make clean all ) &> $(CILOG) & echo $$! > tmp.ci.pid

#stop_ci:
#	kill -9 $(TMPCI)

#start_ct:
#	watch time -p make test & echo $$! > tmp.ct.pid

#stop_ct:
#	kill -9 $(TMPCT)

#start_cd:
#	watch time -p make install & echo $$! > tmp.cd.pid

#stop_cd:
#	kill -9 $(TMPCD)

clean:
	rm -f $(OBJ) $(EXE) $(LOBJ) $(LIB) $(TOBJ) $(TEXE) $(SYSINC) *.tmp $(SDEPS) $(LSDEPS) $(TSDEPS)

#install: $(INC) $(LIB)
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
#	@echo "TMPCI="$(TMPCI)
#	@echo "TMPCT="$(TMPCT)
#	@echo "TMPCD="$(TMPCD)

#	@echo "OS="$(OS)
#	@echo "ARCH="$(ARCH)
#	@echo "MSIZE="$(MSIZE)
#	@echo "SYSINC="$(SYSINC)
#	@echo "DESTDIR="$(DESTDIR)
#	@echo "CFLAGS="$(CFLAGS)
#	@echo "LDFLAGS="$(LDFLAGS)
#	@echo "UNISTD_PATH="$(UNISTD_PATH)
#	@echo "DDIR="$(DDIR)
#	@echo "DSRC="$(DSRC)
#	@echo "SRC="$(SRC)
#	@echo "OBJ="$(OBJ)
#	@echo "HDR="$(HDR)
#	@echo "IDIR="$(IDIR)
#	@echo "INC="$(INC)
#	@echo "EDIR="$(EDIR)
#	@echo "EXE="$(EXE)
#	@echo "LDIR="$(LDIR)
#	@echo "LSRC="$(LSRC)
#	@echo "LOBJ="$(LOBJ)
#	@echo "LNK="$(LNK)
#	@echo "LIB="$(LIB)
#	@echo "TSRC="$(TSRC)
#	@echo "TOBJ="$(TOBJ)
#	@echo "TEXE="$(TEXE)
#	@echo "TMPCI="$(TMPCI)
#	@echo "TMPCT="$(TMPCT)
#	@echo "TMPCD="$(TMPCD)

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
	@awk '/^[a-zA-Z0-9\-_+. ]*: #{2}/ { print; }' $(MAKEFILE_LIST)
