# To add a new configuration for your system add it to the list below
# then update the Make-config file.

default:
	@echo "The enviroment variable FNALROOT must be set to this directory"
	@echo "One time only do a make setup then"
	@echo "Type one of the following:"
	@echo "  make gcc               for a generic system with GCC"
	@echo "  make linux             for Linux systems using GCC"
	@echo "  make irix              for SGI systems with IRIX"
	@echo "  make irix -gcc         for SGI systems with IRIX with GCC"
	@echo "  make irix -debug       for SGI systems with IRIX with GCC debug on"
	@echo "  make solaris           for Solaris systems and Sun compiler"

	@echo "  make solaris-debug     for Solaris systems and Sun debug"
	@echo "  make solaris-devel     for Solaris systems and Sun debug and devel on"
	@echo "  make solaris-gcc       for Solaris systems with GCC"
	@echo "  make solaris-gcc-debug for Solaris systems with GCC debug on"
	@echo "  make setup             Make include and lib dirs and links."
	@echo "  make real-clean        Remove all .o,.sb,.~ files, libraries and links."
	@echo "  make lib-clean         Remove all library (.a) files."
	@echo "  make clean             Remove all .o,.sb,.~ files."
	@echo "  make clean-app         Do a make clean in the app dirs, too"
	@echo "  make clean-all         Do a make clean and a make clean-app"
	@echo "  make link-clean        Remove all links in include directory."
	@echo "  make tar               Tar up all source (*.cc,*.h) files."
	@echo "  make tar_everything    Tar up ALL files."
	@echo "  make app               Does a make (no args) in each app dir."
	@echo "  make test-app          Make all the programs in the app directories."


LINKDIRS  = beamline \
	mxyzptlk \
	machines/Machine \
	machines/tev \
	tcl \
	socket \
	server \
	sybase \
	filter \
	machines/swyd \
	machines/recycler \
	machines/mi_8gev \
	machines/mr_8gev \
	machines/accumulator \
	machines/mi

SUBDIRS  = basic_toolkit/src \
	beamline/src \
	beamline/src/physics/6d \
	beamline/src/physics/mad \
	filter/src \
	mxyzptlk/src \
	machines/accumulator/src \
	machines/Machine/src \
	machines/mi/src \
	machines/mi_8gev/src \
	machines/recycler/src \
	machines/swyd/src \
	machines/tev/src \
	physics_toolkit/src \
	server/src \
	socket/src \
	sybase/src \
	tcl/src

APPDIRS  = filter/app \
	machines/accumulator/app \
	machines/mi/app \
	machines/mi_8gev/app \
	machines/recycler/app \
	machines/swyd/app \
	machines/tev/app \
	server/app \
	sybase/app


gcc solaris-gcc solaris-gcc-debug solaris solaris-debug solaris-devel solaris-depend solaris-gcc-depend:
	@set -x; for i in $(SUBDIRS); do \
		(cd $$i; $(MAKE)  $@); \
		done
	@echo "== MAKE COMPLETE =="

linux irix irix-gcc irix-debug irix-depend:
	@set -x; for i in $(SUBDIRS); do \
		(cd $$i; $(MAKE)  $@ ); \
		done
	@echo "== MAKE COMPLETE =="



setup:
	$(MAKE) lib-dir;

lib-dir:
	@if [ ! -d lib ] ; then \
		mkdir lib;\
		mkdir lib/sun; \
		mkdir lib/sgi; \
		mkdir lib/gcc; \
		else true; \
	fi

real-clean: clean lib-clean

lib-clean:
	rm -f lib/sun/*.a;\
	rm -f lib/sgi/*.a;\
	rm -f lib/gcc/*.a;

# Remove .o files, emacs backup files, etc.
clean:
	rm -f ./*~*
	@set -x; for i in $(SUBDIRS); do \
		(cd $$i; $(MAKE)  $@); \
		done
	@echo "== CLEAN COMPLETE =="

clean-app: 
	@set -x; for i in $(APPDIRS); do \
		(cd $$i; $(MAKE) clean); \
		done
	@echo "== CLEAN-APP COMPLETE =="

clean-all: clean clean-app

link-clean:
	rm -f include/*

export: real-clean
	rm -rf lib;

NAME = Model-2.0

TAR_FILES =	\
	fnal/README \
	fnal/Makefile	\
	fnal/Make-config \
	fnal/tar-exclude \
	fnal/ChangeLog \
	fnal/beamline	\
	fnal/mxyzptlk	\
	fnal/machines \
	fnal/server \
	fnal/socket \
	fnal/sybase \
	fnal/filter \
	fnal/tcl

# The tar command relies on the Solaris tar; -FF won't work with gnu tar.
# The location of tar in irix seems to be /bin/tar.  Tar is probably not
# available on PC's either, but one might want to use some zip derivative.

tar:
	cd .. ; \
	/usr/sbin/tar -cvfFFX /tmp/$(NAME).tar fnal/tar-exclude $(TAR_FILES) ; \
	gzip /tmp/$(NAME).tar ; \
	mv /tmp/$(NAME).tar.gz fnal

tar_everything:
	cd .. ; \
	/usr/sbin/tar -cvf $(NAME).tar $(TAR_FILES) fnal/mc ; \
	gzip $(NAME).tar ; \
	mv $(NAME).tar.gz fnal

test-app:
	@set -x; for i in $(APPDIRS); do \
		(cd $$i; $(MAKE) clean; $(MAKE) all); \
		done
	@echo "== MAKE test-app COMPLETE =="
