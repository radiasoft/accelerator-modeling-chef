.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) -I$(TKINC) -I$(XINC)

DEPEND_INC = $(INCLUDEDIR) -I$(COMPILER_DEPEND)


OBJS =  BBLensPhysics.o\
	   JetQuadrupolePhysics.o\
	   JetThinQuadPhysics.o\
	   JetthinSextPhysics.o\
	   bmlnElmntPhysics.o \
	   combinedFuncPhys.o \
           hkickPhysics.o \
           hmonitorPhysics.o \
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
           thinQuadPhysics.o \
	   thinSeptumPhysics.o\
           thinSextupolePhysics.o \
           thinrfcavityPhysics.o \
           vkickPhysics.o \
           vmonitorPhysics.o

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
	rm -rf .sb

depend:
	makedepend -- $(DEPEND_INC) -- $(SOURCES)


# DO NOT DELETE THIS LINE -- make depend depends on it.

