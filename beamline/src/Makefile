.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) -I$(PHYSICS_TOOLKIT_INC) -I$(SOCKET_INC) -I$(TKINC) -I$(XINC)

OBJS = BBLens.o \
       BmlVisitor.o \
       CF_rbend.o \
       InsertionList.o \
       JetQuadrupole.o\
       JetThinQuad.o\
       JetthinSext.o\
       InsertionList.o \
       Particle.o \
       ParticleBunch.o \
       Slot.o \
       alignment.o \
       beamline.o \
       BeamlineIterator.o \
       bmlnElmnt.o \
       circuit.o \
       combinedFunction.o \
       drift.o \
       hkick.o \
       hmonitor.o \
       kick.o \
       marker.o \
       monitor.o \
       mwiremonitor.o \
       octupole.o \
       pinger.o \
       quadrupole.o \
       rastermonitor.o \
       rbend.o \
       sbend.o \
       sector.o \
       sextupole.o \
       srot.o \
       tclmonitor.o \
       thin12pole.o \
       thin14pole.o \
       thin16pole.o \
       thin18pole.o \
       thinDecapole.o \
       thinLamb.o \
       thinMultipole.o \
       thinOctupole.o \
       thinQuad.o \
       thinSeptum.o \
       thinSextupole.o \
       thinrfcavity.o \
       vkick.o \
       vmonitor.o \
       read_istream.o

SOURCES = $(OBJS:.o=.cc)

.cc.o:
	$(C++) $(C++FLAGS) $(INCLUDEDIR)  -c $*.cc
.c.o:
	$(C) $(CFLAGS) $(INCLUDEDIR) -c $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libbeamline.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libbeamline.a

clean:
	rm -f *.o *~*
	rm -f core
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

real-clean: clean
	rm -f dependencies
	touch dependencies

depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc > dependencies


include dependencies
