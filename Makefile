# To add a new configuration for your system add it to the list below
# then update the Make-config file.

default:
	@echo "The enviroment variable FNALROOT must be set to this directory"
	@echo "One time only do a make setup then"
	@echo "Type one of the following:"
	@echo "  make gcc               for a generic system with GCC"
	@echo "  make linux             for Linux systems using GCC"
	@echo "  make irix5             for SGI systems with IRIX 5.x"
	@echo "  make irix5-gcc         for SGI systems with IRIX 5.x with GCC"
	@echo "  make irix5-debug       for SGI systems with IRIX 5.x with GCC debug on"
	@echo "  make solaris           for Solaris systems and Sun compiler"
	@echo "  make ultra             for Solaris systems on Ultras with the Sun compiler"
	@echo "  make ultra-opt         for Solaris Ultra systems with the Sun compiler and \
                                        -fast optimization"
	@echo "  make ultra-debug       for Solaris Ultra systems with the Sun compiler and \
                                        debug on"
	@echo "  make solaris-debug     for Solaris systems and Sun debug"
	@echo "  make solaris-gcc       for Solaris systems with GCC"
	@echo "  make solaris-gcc-debug for Solaris systems with GCC debug on"
	@echo "  make setup             Make include and lib dirs and links."
	@echo "  make link              Make the links in the include dir."
	@echo "  make include-dir       Make the include directory."
	@echo "  make lib-dir           Make the library directory tree."
	@echo "  make real-clean        Remove all .o,.sb,.~ files, libraries and links."
	@echo "  make lib-clean         Remove all library (.a) files."
	@echo "  make clean             Remove all .o,.sb,.~ files."
	@echo "  make link-clean        Remove all links in include directory."
	@echo "  make tar               Tar up all source (*.cc,*.h) files."
	@echo "  make tar_everything    Tar up ALL files."


LINKDIRS  = bml mxb mi Machine tev tcl socket server sybase filter swyd recycler mi_8gev mr_8gev 

SUBDIRS  = bml/src bml/src/physics/6d  bml/src/physics/mad mxb/src Machine/src tev/src Machine/src tcl/src  socket/src sybase/src sybase/app tev/app server/app server/src filter/src filter/app swyd/src swyd/app recycler/app recycler/src mi_8gev/src mi_8gev/app 

SUNSUBDIRS  = bml/src bml/src/physics/6d  bml/src/physics/mad mxb/src Machine/src tev/src tcl/src socket/src sybase/src server/src filter/src swyd/src recycler/src mi_8gev/src

SGISUBDIRS  = $(SUNSUBDIRS)

gcc solaris-gcc solaris-gcc-debug solaris solaris-debug ultra ultra-opt ultra-debug :
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

NAME = Model-2.0

TAR_FILES =	\
	fnal/README \
	fnal/Makefile	\
	fnal/Make-config \
	fnal/tar-exclude \
	fnal/ChangeLog \
	fnal/bml	\
	fnal/mxb	\
	fnal/tev \
	fnal/Machine \
	fnal/server \
	fnal/socket \
	fnal/sybase \
	fnal/filter \
	fnal/swyd \
	fnal/recycler \
	fnal/tcl \
	fnal/mi \
	fnal/mi_8gev \
	fnal/mr_8gev 

tar:
	cd .. ; \
	tar -cvfFFX $(NAME).tar fnal/tar-exclude $(TAR_FILES) ; \
	gzip $(NAME).tar ; \
	mv $(NAME).tar.gz fnal

tar_everything:
	cd .. ; \
	tar -cvf $(NAME).tar $(TAR_FILES) fnal/mc ; \
	gzip $(NAME).tar ; \
	mv $(NAME).tar.gz fnal


