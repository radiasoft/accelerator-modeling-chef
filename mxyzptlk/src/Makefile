.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC)

DEPEND_INC = $(INCLUDEDIR) -I$(COMPILER_DEPEND)



OBJS = CLieOperator.o \
        JL.o \
        JLC.o \
        Jet.o \
        JetC.o \
        JetCFcns.o \
        JetCVector.o \
        JetConversions.o \
        JetFcns.o \
        JetVector.o \
        LieOperator.o \
        MLJ.o \
        Mapping.o \
        MappingC.o \
        MatrixJ.o 


SOURCES = $(OBJS:.o=.cc)


.cc.o:
	$(C++) -c $(INCLUDEDIR) $(C++FLAGS) $*.cc
.c.o:
	$(C) -c $(INCLUDEDIR) $(CFLAGS) $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libmxyzptlk.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libmxyzptlk.a

clean:
	rm -f *.o *~*
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

depend:
	makedepend -- $(DEPEND_INC) -- $(SOURCES)


# DO NOT DELETE THIS LINE -- make depend depends on it.

