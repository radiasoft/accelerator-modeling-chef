.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) -I$(PHYSICS_TOOLKIT_INC) -I$(SOCKET_INC) -I$(TKINC) -I$(XINC) $(STL_CONFIG)

OBJS = BBLens.o \
       BmlVisitor.o \
       CF_rbend.o \
       CF_sbend.o \
       InsertionList.o \
       JetQuadrupole.o\
       JetthinSext.o\
       InsertionList.o \
       MappPropFunc.o \
       Particle.o \
       ParticleBunch.o \
       Slot.o \
       alignment.o \
       beamline.o \
       BeamlineIterator.o \
       BeamlineExpressionTree.o \
       bmlnElmnt.o \
       circuit.o \
       ICircuit.o \
       FCircuit.o \
       mover.o \
       BipolarCircuit.o \
       combinedFunction.o \
       drift.o \
       kick.o \
       marker.o \
       monitor.o \
       octupole.o \
       pinger.o \
       quadrupole.o \
       rbend.o \
       sbend.o \
       sector.o \
       sextupole.o \
       srot.o \
       thinpoles.o \
       decapole.o \
       lambertson.o \
       septum.o \
       rfcavity.o \
       read_istream.o

SOURCES = $(OBJS:.o=.cc)

.cc.o:
	$(C++) $(C++FLAGS) -D_EXCLUDE_MONITORS_ $(INCLUDEDIR)  -c $*.cc
.c.o:
	$(C) $(CFLAGS) -D_EXCLUDE_MONITORS_ $(INCLUDEDIR) -c $*.c

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
