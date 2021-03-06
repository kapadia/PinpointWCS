#
# This file is a Makefile for XPA.  If it has the name "Makefile.in"
# then it is a template for a Makefile;  to generate the actual Makefile,
# run "./configure", which is a configuration script generated by the
# "autoconf" program (constructs like "@foo@" will get replaced in the
# actual Makefile.
#

PACKAGE = xpa
VERSION = 2.1.13

DISTNAME = xpa-${VERSION}
DISTDIR = ../export/${DISTNAME}
FTPDIR	= ../ftp

#----------------------------------------------------------------
# Things you can change to personalize the Makefile for your own
# site (you can make these changes in either Makefile.in or
# Makefile, but changes to Makefile will get lost if you re-run
# the configuration script).
#----------------------------------------------------------------

# Default top-level directories in which to install architecture-
# specific files (exec_prefix) and machine-independent files such
# as scripts (prefix).  The values specified here may be overridden
# at configure-time with the --exec-prefix and --prefix options
# to the "configure" script.

prefix =		/home/akapadia/Documents/pinpointwcs/xpa-2.1.13
exec_prefix =		${prefix}

# The following definition can be set to non-null for special systems
# like AFS with replication.  It allows the pathnames used for installation
# to be different than those used for actually reference files at
# run-time.  INSTALL_ROOT is prepended to $prefix and $exec_prefix
# when installing files.
INSTALL_ROOT =

# Directory in which to install the .a or .so binary for the XPA library:
LIB_INSTALL_DIR =	$(INSTALL_ROOT)$(exec_prefix)/lib

# Directory in which to install the program wish:
BIN_INSTALL_DIR =	$(INSTALL_ROOT)$(exec_prefix)/bin

# Directory in which to install the include file xpa.h:
INCLUDE_INSTALL_DIR =	$(INSTALL_ROOT)$(prefix)/include

# Top-level directory for manual entries:
MAN_INSTALL_DIR =	$(INSTALL_ROOT)$(prefix)/man

# Top-level directory for share entries:
MAN_SHARE_DIR =		$(INSTALL_ROOT)$(prefix)/share/xpa

# Platform-specific X compiler flags (include file specifications)
X_CFLAGS = 		
# Platform-specific link directive for X libraries (-L specification)
X_LIBS = 		
# Platform-specific libraries that must go before -lXt
X_PRE_LIBS = 		
# Platform-specific libraries that must go after -lXt
X_EXTRA_LIBS = 		

# Platform-specific include files for Tcl
TCL_CFLAGS = 		
# Platform-specific libraries for Tcl
TCL_LIBS = 		

# Platform-specific include files for Gtk
GTK_CFLAGS = 		
# Platform-specific libraries for Gtk
GTK_LIBS = 		

# combine package cflags
PKG_CFLAGS =	$(X_CFLAGS) $(TCL_CFLAGS) $(GTK_CFLAGS)

# extra Libs required to link (e.g. socket libraries)
LIBS =			

# pthread lib needed to xpans
TLIB =			

# To change the compiler switches, for example to change from -O
# to -g, change the following line:
CFLAGS = 		-g -O2

# To add ld switches, change the following line:
LDFLAGS =		 

# Some versions of make, like SGI's, use the following variable to
# determine which shell to use for executing commands:
SHELL =			/bin/sh

# if enabled-shared was specified, this will exand to "shlib" and trigger
# building of the shared library
DOSHARED =	

# There are just too many different versions of "install" around;
# better to use the install-sh script that comes with the distribution,
# which is slower but guaranteed to work.

INSTALL = 		./install-sh -c
INSTALL_PROGRAM =	${INSTALL}
INSTALL_DATA =		${INSTALL} -m 644

#----------------------------------------------------------------
# The information below is modified by the configure script when
# Makefile is generated from Makefile.in.  You shouldn't normally
# modify any of this stuff by hand.
#----------------------------------------------------------------

AC_FLAGS =		-DHAVE_CONFIG_H
RANLIB =		ranlib
EXE =			

#----------------------------------------------------------------
# The information below should be usable as is.  The configure
# script won't modify it and you shouldn't need to modify it
# either.
#----------------------------------------------------------------
RM =		rm -f

CC =		gcc

CC_SWITCHES =	-I. ${CFLAGS} ${AC_FLAGS}

DEPEND_SWITCHES = -I. ${CFLAGS} ${PKG_CFLAGS} ${AC_FLAGS}

SRCS =		xpa.c xpaio.c command.c acl.c remote.c clipboard.c port.c \
		tcp.c client.c word.c xalloc.c find.c xlaunch.c timedconn.c \
		tclloop.c tcl.c xtloop.c gtkloop.c \
		xpaset.c xpaget.c xpainfo.c xpaaccess.c xpans.c xpamb.c

BASE_OBJS =	xpa.o xpaio.o command.o acl.o remote.o clipboard.o port.o \
		tcp.o client.o word.o xalloc.o find.o xlaunch.o timedconn.o

TCL_OBJS =	tclloop.o tcl.o 

XT_OBJS =	xtloop.o

GTK_OBJS =	gtkloop.o

INCL	=	xpa.h xpap.h

# these are all the modules going into the "normal" xpa library
LIBOBJS =	${BASE_OBJS} ${TCL_OBJS} ${XT_OBJS} ${GTK_OBJS}
LIB =	libxpa.a

# used in link line
# LLIB=	$(LIB)
LLIB=	$(LIB)

PROGS =		xpaset xpaget xpainfo xpaaccess xpans xpamb

TESTPROGS =	ctest stest rtest

all:		xpa.h lib $(PROGS)

testall:	$(TESTPROGS)

All:		all testall

install::	install-binaries 

install::	$(DOSHARED)_install

install::	install-man 

install::	install-share

lib:		$(LIB) $(DOSHARED)

$(LIB):		$(LIBOBJS)
		$(RM) $(LIB)
		ar crv $(LIB) $(LIBOBJS)
		$(RANLIB) $(LIB)

shlib:		$(LIB)
		@(rm -rf lib$(PACKAGE).tmp; mkdir lib$(PACKAGE).tmp; \
		(cd lib$(PACKAGE).tmp && ar x ../lib$(PACKAGE).a); \
		rm -f lib$(PACKAGE).tmp/xt*.o; \
		rm -f lib$(PACKAGE).tmp/tcl*.o; \
		CC='$(CC)' CXX=$(CXX) \
		./mklib -o $(PACKAGE) lib$(PACKAGE).tmp/*.o; \
		rm -rf lib$(PACKAGE).tmp)

mingw-dll:	$(LIBOBJS)
		$(CC)  -I. -DHAVE_CONFIG_H -shared port.c  tcp.c acl.c find.c \
		remote.c timedconn.c client.c word.c xpaio.c clipboard.c \
		launch.c xalloc.c command.c xpa.c \
		LibMain.c  \
		-e _LibMain@12 -Wl,--image-base,0x1C000000 -o libxpa.dll \
		-luser32 -lwsock32

tclxpa:		$(LIB)
		@(rm -rf libtclxpa.tmp; mkdir libtclxpa.tmp; \
		(cd libtclxpa.tmp && ar x ../lib$(PACKAGE).a); \
		rm -f libtclxpa.tmp/xt*.o; \
		CC='$(CC)' CXX=$(CXX) \
		./mklib -o tclxpa libtclxpa.tmp/*.o $(TCL_LIBS); \
		test -r libtclxpa.dylib && cp -p libtclxpa.dylib libtclxpa.so && echo "copying libtclxpa.dylib to libtclxpa.so"; \
		rm -rf libtclxpa.tmp)

diff:
		-(for f in `ls *.c`;					\
		    do							\
			echo $$f;					\
			diff /soft/saord/xpa-2.1.[0-9]*/$$f .;		\
		    done);

index:		$(LIB)
		@(test -r pkgIndex.tcl && mv pkgIndex.tcl pkgIndex.tcl-old; \
		echo "pkg_mkIndex -direct -verbose . libtclxpa.so; exit"| tclsh)

xpaset:		$(LIB) xpaset.o
		$(CC) $(LDFLAGS) xpaset.o -o xpaset $(LLIB) $(LIBS)

xpaget:		$(LIB) xpaget.o
		$(CC) $(LDFLAGS) xpaget.o -o xpaget $(LLIB) $(LIBS)

xpainfo:	$(LIB) xpainfo.o
		$(CC) $(LDFLAGS) xpainfo.o -o xpainfo $(LLIB) $(LIBS)

xpaaccess:	$(LIB) xpaaccess.o
		$(CC) $(LDFLAGS) xpaaccess.o -o xpaaccess $(LLIB) $(LIBS)

xpans:		$(LIB) xpans.o
		$(CC) $(LDFLAGS) xpans.o -o xpans $(LLIB) $(LIBS) $(TLIB)

xpamb:		$(LIB) xpamb.o
		$(CC) $(LDFLAGS) xpamb.o -o xpamb $(LLIB) $(LIBS)

ctest:		$(LIB) ctest.o
		$(CC) $(LDFLAGS) ctest.o -o ctest $(LLIB) $(LIBS)

stest:		$(LIB) stest.o
		$(CC) $(LDFLAGS) stest.o -o stest $(LIB) $(LIBS)

rtest:		$(LIB) rtest.o
		$(CC) $(LDFLAGS) rtest.o -o rtest $(LIB) $(LIBS)

stestx:		$(LIB) stest.o
		$(CC) $(LDFLAGS) stest.o -o stest $(LIB) \
		$(X_LIBS) -lXt $(X_PRE_LIBS) -lXext -lX11 $(LIBS)

# Smoke test: allows end-users to quickly discern basic usability
smoke:  ctest stest
	./stest smoke &
	sleep 3
	./ctest -e -b -l 1000 smoke
	./xpaset -p smoke exit

# Note: before running ranlib below, must cd to target directory because
# some ranlibs write to current directory, and this might not always be
# possible (e.g. if installing as root).

# this nop-op gets executed if we are not building shared libraries
_install:

shlib_install:
	@-(for i in `ls *.so* *.dylib *.sl 2>/dev/null` ; \
	   do \
	   if [ -h $$i ] ; then \
		echo "Installing link $$i" ; \
		tar cf - $$i | (cd $(LIB_INSTALL_DIR); tar xf -) ; \
	   else \
		echo "Installing $$i" ; \
		$(INSTALL_DATA) $$i $(LIB_INSTALL_DIR)/$$i ; \
		chmod 555 $(LIB_INSTALL_DIR)/$$i; \
	   fi; \
	   done;)

install-binaries: $(LIB) $(PROGS)
	@for i in $(LIB_INSTALL_DIR) $(INCLUDE_INSTALL_DIR) $(BIN_INSTALL_DIR) ; \
	    do \
	    if [ ! -d $$i ] ; then \
		echo "Making directory $$i"; \
		mkdir $$i; \
		chmod 755 $$i; \
		else true; \
		fi; \
	    done;
	@echo "Installing $(LIB)";
	@$(INSTALL_DATA) $(LIB) $(LIB_INSTALL_DIR)/$(LIB);
	@(cd $(LIB_INSTALL_DIR); $(RANLIB) $(LIB));
	@chmod 555 $(LIB_INSTALL_DIR)/$(LIB);
	@echo "Installing xpa.h"
	@$(INSTALL_DATA) xpa.h $(INCLUDE_INSTALL_DIR)/xpa.h
	@echo "Installing prsetup.h"
	@$(INSTALL_DATA) prsetup.h $(INCLUDE_INSTALL_DIR)/prsetup.h
	@for i in $(PROGS) ; \
	    do \
	        echo "Installing $$i$(EXE)" ; \
		$(INSTALL_PROGRAM) $$i$(EXE) $(BIN_INSTALL_DIR)/$$i$(EXE) ; \
	    done;

install-man:
	@if [ ! -d $(MAN_INSTALL_DIR) ] ; then \
	    echo "Making directory $(MAN_INSTALL_DIR)"; \
	    mkdir $(MAN_INSTALL_DIR); \
	    chmod 755 $(MAN_INSTALL_DIR); \
	    else true; \
	fi;
	@-(for i in `ls ./man/man?/*.?` ; \
	    do \
		B=`basename $$i`; \
		E=`echo $$i | awk -F. '{print $$NF}'`; \
		M="$(MAN_INSTALL_DIR)/man$$E"; \
		if [ ! -d $$M ] ; then \
		    echo "Making directory $$M"; \
		    mkdir $$M; \
		    chmod 755 $$M; \
		    else true; \
		fi; \
		echo "Installing $$B" ; \
		$(INSTALL_DATA) $$i $$M/$$B; \
	    done;)

install-share:
	@if [ ! -d $(MAN_SHARE_DIR) ] ; then \
	    echo "Making directory $(MAN_SHARE_DIR)"; \
	    mkdir -p $(MAN_SHARE_DIR); \
	    chmod 755 $(MAN_SHARE_DIR); \
	    else true; \
	fi;
	@-(for i in `ls ./doc/sman/xpa?.*` ; \
	    do \
		B=`basename $$i`; \
		echo "Installing $$B" ; \
		$(INSTALL_DATA) $$i $(MAN_SHARE_DIR)/$$B; \
	    done;)

Makefile:	Makefile.in
		$(SHELL) config.status

clean:
		$(RM) *.a *.so *.so.* *.dylib *.o *.exe core \
			errs *pure* .nfs* \
			foo* *~ *.log \#* TAGS *.E a.out errors \
			$(PROGS) $(TESTPROGS) \
			gmon.out *.pg *.bak \
			config.info config.log \
			doc/*~ doc/*.bak man/*~
		$(RM) -r autom4te.cache a.out.dSYM

distclean:	clean
		$(RM) Makefile config.status config.cache config.log

maintainer-clean:: clean
		$(RM) config.status config.cache config.log

depend:
		makedepend -- $(DEPEND_SWITCHES) -- $(SRCS)

acl.o:		acl.c $(INCL)
		$(CC) -c $(CC_SWITCHES) acl.c

remote.o:	remote.c $(INCL)
		$(CC) -c $(CC_SWITCHES) remote.c

clipboard.o:	clipboard.c $(INCL)
		$(CC) -c $(CC_SWITCHES) clipboard.c

client.o:	client.c $(INCL)
		$(CC) -c $(CC_SWITCHES) client.c

command.o:	command.c $(INCL)
		$(CC) -c $(CC_SWITCHES) command.c

find.o:		find.c
		$(CC) -c $(CC_SWITCHES) find.c

xlaunch.o:	xlaunch.c
		$(CC) -c $(CC_SWITCHES) xlaunch.c

timedconn.o:	timedconn.c
		$(CC) -c $(CC_SWITCHES) timedconn.c

port.o:		port.c $(INCL)
		$(CC) -c $(CC_SWITCHES) port.c

tcl.o:		tcl.c $(INCL)
		$(CC) -c $(CC_SWITCHES) $(TCL_CFLAGS) tcl.c

tclloop.o:	tclloop.c $(INCL)
		$(CC) -c $(CC_SWITCHES) $(TCL_CFLAGS) tclloop.c

gtkloop.o:	gtkloop.c $(INCL)
		$(CC) -c $(CC_SWITCHES) $(GTK_CFLAGS) gtkloop.c

tcp.o:		tcp.c
		$(CC) -c $(CC_SWITCHES) tcp.c

word.o:		word.c
		$(CC) -c $(CC_SWITCHES) word.c

xalloc.o:	xalloc.c
		$(CC) -c $(CC_SWITCHES) xalloc.c

xpa.o:		xpa.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpa.c

xpaio.o:	xpaio.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpaio.c

xtloop.o:	xtloop.c $(INCL)
		$(CC) -c $(CC_SWITCHES) $(X_CFLAGS) xtloop.c

xpaaccess.o:	xpaaccess.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpaaccess.c

xpaget.o:	xpaget.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpaget.c

xpainfo.o:	xpainfo.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpainfo.c

xpans.o:	xpans.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpans.c

xpamb.o:	xpamb.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpamb.c

xpaset.o:	xpaset.c $(INCL)
		$(CC) -c $(CC_SWITCHES) xpaset.c

ctest.o:	ctest.c
		$(CC) -c $(CC_SWITCHES) ctest.c

stest.o:	stest.c
		$(CC) -c $(CC_SWITCHES) $(X_CFLAGS) stest.c

rtest.o:	rtest.c
		$(CC) -c $(CC_SWITCHES) $(X_CFLAGS) rtest.c

stestx.o:	stest.c
		$(CC) -o stest.o -c $(CC_SWITCHES) $(X_CFLAGS) \
		-DBUILD_WITH_XT stest.c

strstr.o:	./compat/strstr.c
		$(CC) -c $(CC_SWITCHES) -o strstr.o ./compat/strstr.c

xpa.h:		configure.ac
		@($(RM) -r oxpa.h; \
		MAJOR=`echo "${VERSION}" | awk -F. '{print $$1}'`; \
		MINOR=`echo "${VERSION}" | awk -F. '{print $$2}'`; \
		PATCH=`echo "${VERSION}" | awk -F. '{print $$3}'`; \
		sed "s/^#define XPA_VERSION.*/#define XPA_VERSION \"$(VERSION)\"/;s/^#define XPA_MAJOR_VERSION.*/#define XPA_MAJOR_VERSION $${MAJOR}/;s/^#define XPA_MINOR_VERSION.*/#define XPA_MINOR_VERSION $${MINOR}/;s/^#define XPA_PATCH_LEVEL.*/#define XPA_PATCH_LEVEL $${PATCH}/;" < xpa.h > nxpa.h; \
		mv xpa.h oxpa.h; \
		mv nxpa.h xpa.h)

configure:	configure.ac
		autoconf

dist:   	configure
		($(RM) -r $(DISTDIR); \
		mkdir $(DISTDIR); \
		cp -p *.[ch] *.tcl $(DISTDIR)/.; \
		cp -p pkgIndex.tcl $(DISTDIR)/.; \
		cp -p Makefile.in $(DISTDIR)/.; \
		chmod 664 $(DISTDIR)/Makefile.in; \
		cp -p conf.h.in $(DISTDIR)/.; \
		chmod 664 $(DISTDIR)/conf.h.in; \
		cp -p configure.ac $(DISTDIR)/.; \
		chmod 644 $(DISTDIR)/configure.ac; \
		cp -p  *.m4 $(DISTDIR)/.; \
		chmod 644 $(DISTDIR)/*.m4; \
		cp -p configure $(DISTDIR)/.; \
		chmod 755 $(DISTDIR)/configure; \
		cp -p config.sub config.guess $(DISTDIR)/.; \
		chmod 755 $(DISTDIR)/config.sub $(DISTDIR)/config.guess; \
		cp -p saoconfig $(DISTDIR)/.; \
		chmod 755 $(DISTDIR)/saoconfig; \
		cp -p mklib $(DISTDIR)/.; \
		chmod 755 $(DISTDIR)/mklib; \
		cp -p install-sh $(DISTDIR)/install-sh; \
		chmod 755 $(DISTDIR)/install-sh; \
		cp -p README INSTALL COPYING $(DISTDIR)/.; \
		mkdir $(DISTDIR)/doc; \
		cp -p ./doc/*.html $(DISTDIR)/doc/.; \
		cp -p ./doc/*.ps ./doc/*.pdf $(DISTDIR)/doc/.; \
		cp -p ./doc/Makefile $(DISTDIR)/doc/.; \
		mkdir $(DISTDIR)/doc/sman; \
		cp -p ./doc/sman/* $(DISTDIR)/doc/sman/.; \
		mkdir $(DISTDIR)/man; \
		cp -p -R ./man/* $(DISTDIR)/man/.)

release:	dist
		(cd $(DISTDIR); cd ..; \
		tar cf - $(DISTNAME) | \
		gzip -9 -c > $(FTPDIR)/$(DISTNAME).tar.gz)

tar:
		($(RM) config.cache; \
		cd ..; \
		tar cf - $(DISTNAME) | gzip -9 -c > $(DISTNAME).tar.gz)

errcheck:
		@-egrep 'error|warning' foo

itar:
		(cd doc/sman; \
		tar cf - . | gzip -9 -c > ../../../$(DISTNAME)-iman.tar.gz)

sman:
		@(cd doc && $(MAKE) index)

docs:
		@(cd doc; $(MAKE))

pure:		xpaset.pure xpaget.pure xpainfo.pure xpaaccess.pure \
		xpans.pure xpamb.pure ctest.pure stest.pure rtest.pure

xpaset.pure:	$(LIB) xpaset.o
		purify $(CC) $(LDFLAGS) xpaset.o -o xpaset.pure \
		$(LLIB) $(LIBS)

xpaget.pure:	$(LIB) xpaget.o
		purify $(CC) $(LDFLAGS) xpaget.o -o xpaget.pure \
		$(LLIB) $(LIBS)

xpainfo.pure:	$(LIB) xpainfo.o
		purify $(CC) $(LDFLAGS) xpainfo.o -o xpainfo.pure \
		$(LLIB) $(LIBS)

xpaaccess.pure:	$(LIB) xpaaccess.o
		purify $(CC) $(LDFLAGS) xpaaccess.o -o xpaaccess.pure \
		$(LLIB) $(LIBS)

xpans.pure:	$(LIB) xpans.o
		purify $(CC) $(LDFLAGS) xpans.o -o xpans.pure \
		$(LLIB) $(LIBS) $(TLIB)

xpamb.pure:	$(LIB) xpamb.o
		purify $(CC) $(LDFLAGS) xpamb.o -o xpamb.pure \
		$(LLIB) $(LIBS)

ctest.pure:	$(LIB) ctest.o
		purify $(CC) $(LDFLAGS) ctest.o -o ctest.pure \
		$(LLIB) $(LIBS)

stest.pure:	$(LIB) stest.o
		purify $(CC) $(LDFLAGS) stest.o -o stest.pure $(LIB) \
		$(X_LIBS) -lXt $(X_PRE_LIBS) -lXext -lX11 $(LIBS)

rtest.pure:	$(LIB) rtest.o
		purify $(CC) $(LDFLAGS) rtest.o -o rtest.pure \
		$(LLIB) $(LIBS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
