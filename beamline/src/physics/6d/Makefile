.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) -I$(TKINC) -I$(XINC) $(STL_CONFIG)

OBJS =  BBLensPhysics.o\
	   JetQuadrupolePhysics.o\
	   JetthinSextPhysics.o\
	   bmlnElmntPhysics.o \
	   combinedFuncPhys.o \
	   kickPhysics.o \
           markerPhysics.o \
           monitorPhysics.o \
           octupolePhysics.o \
           PingerPhysics.o \
           quadrupolePhysics.o \
           rbendPhysics.o \
           sbendPhysics.o \
           sectorPhysics.o \
           sextupolePhysics.o \
           srotPhysics.o \
           thin12polePhysics.o \
           thin14polePhysics.o \
           thin16polePhysics.o \
           thin18polePhysics.o \
           thinDecapolePhysics.o \
	   thinLambPhysics.o \
           thinMultipolePhysics.o \
           thinOctupolePhysics.o \
	   thinSeptumPhysics.o\
           thinSextupolePhysics.o \
           thinrfcavityPhysics.o

SOURCES = $(OBJS:.o=.cc)

.cc.o:
	$(C++) -c  $(C++FLAGS) $(INCLUDEDIR) $*.cc
.c.o:
	$(C) -c $(CFLAGS) $(INCLUDEDIR) $*.c

include $(FNALROOT)/Make-config


all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libbeamline.a  $(OBJS)
	$(RANLIB) $(LIBDIR)/libbeamline.a


clean:
	rm -f *.o *~*
	rm -f core
	rm -rf .sb

real-clean: clean
	rm -f dependencies
	touch dependencies

depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies
