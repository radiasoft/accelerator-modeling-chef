# To add a new configuration for your system add it to the list below
# then update the Make-config file.

default:
	@echo "The enviroment variable FNALROOT must be set to this directory"
	@echo "One time only do a make setup then"
	@echo "Type one of the following:"
	@echo "   make gcc            for a generic system with GCC"
	@echo "   make irix5          for SGI systems with IRIX 5.x"
	@echo "   make irix5-gcc      for SGI systems with IRIX 5.x with GCC"
	@echo "   make irix5-gcc      for SGI systems with IRIX 5.x with GCC debug on"
	@echo "   make solaris-gcc    for Solaris 2 systems with GCC"
	@echo "   make solaris        for Solaris 2 systems "
	@echo "   make solaris-debug  for Solaris 2 systems and Sun debug"
	@echo "   make sunos4         for Sun systems with SunOS 4.x"
	@echo "   make sunos4-debug   for Sun systems with SunOS 4.x and Sun debug"
	@echo "   make clean"



LINKDIRS  = beamline mxyzptlk Machine tev tcl socket server sybase filter swyd

SUBDIRS  = beamline/src beamline/src/physics/6d  beamline/src/physics/mad mxyzptlk/src Machine/src tev/src Machine/src tcl/src  socket/src sybase/src sybase/app tev/app server/app server/src filter/src filter/app swyd/src swyd/app

SUNSUBDIRS  = beamline/src beamline/src/physics/6d  beamline/src/physics/mad mxyzptlk/src Machine/src tev/src tcl/src socket/src sybase/src server/src filter/src swyd/src

SGISUBDIRS  = beamline/src beamline/src/physics/6d  beamline/src/physics/mad mxyzptlk/src Machine/src tev/src sybase/src socket/src server/src filter/src

gcc solaris-gcc solaris solaris-debug sunos4 sunos4-debug ultra ultra-opt ultra-debug :
	@set -x; for i in $(SUNSUBDIRS); do \
		(cd $$i; $(MAKE)  $@); \
		done
	@echo "== MAKE COMPLETE =="

linux irix5 irix5-gcc irix5-debug :
	@set -x; for i in $(SGISUBDIRS); do \
		(cd $$i; $(MAKE)  $@ ); \
		done
	@echo "== MAKE COMPLETE =="

setup:
	$(MAKE) include-dir;\
	$(MAKE) lib-dir;\
	$(MAKE) link

link:
	@set -x; for i in $(LINKDIRS); do \
		(cd $$i; $(MAKE)  $@ ); \
		done
	@echo "== LINK COMPLETE =="

include-dir:
	@if [ ! -d include ] ; then \
		mkdir include; \
		else true; \
	fi

lib-dir:
	@if [ ! -d lib ] ; then \
		mkdir lib;\
		mkdir lib/ultra; \
		mkdir lib/sun; \
		mkdir lib/sgi; \
		mkdir lib/gcc; \
		else true; \
	fi
	

real-clean: clean lib-clean link-clean

lib-clean:
	rm -f lib/ultra/*.a;\
	rm -f lib/sun/*.a;\
	rm -f lib/sgi/*.a;\
	rm -f lib/gcc/*.a;
	
# Remove .o files, emacs backup files, etc.

clean:
	rm -f ./*~*
	rm -f include/*~*
	@set -x; for i in $(SUBDIRS); do \
		(cd $$i; $(MAKE)  $@); \
		done
	@echo "== CLEAN COMPLETE =="

link-clean:
	rm -f include/*

export: real-clean
	rm -rf include;\
	rm -rf lib;\

NAME = Model-1.0

TAR_FILES =	\
	fnal/README \
	fnal/Makefile	\
	fnal/Make-config \
	fnal/tar-exclude \
	fnal/ChangeLog \
	fnal/beamline	\
	fnal/mxyzptlk	\
	fnal/tev \
	fnal/Machine \
	fnal/server \
	fnal/socket \
	fnal/sybase \
	fnal/filter \
	fnal/swyd \
	fnal/tcl

tar:
	cd .. ; \
	tar -cvfX $(NAME).tar fnal/tar-exclude $(TAR_FILES) ; \
	gzip $(NAME).tar ; \
	mv $(NAME).tar.gz fnal

tar_everything:
	cd .. ; \
	tar -cvf $(NAME).tar $(TAR_FILES) fnal/mc fnal/mxb ; \
	gzip $(NAME).tar ; \
	mv $(NAME).tar.gz fnal


