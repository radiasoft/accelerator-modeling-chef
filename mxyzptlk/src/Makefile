.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC)

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
	rm -f core
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

real-clean: clean
	rm -f dependencies
	touch dependencies


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies

