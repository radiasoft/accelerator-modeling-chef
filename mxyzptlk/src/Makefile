.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC)

DEPEND_INC = $(INCLUDEDIR) -I$(COMPILER_DEPEND)



MXOBJS = CLieOperator.o \
        ConvertToTunes.o \
        IntArray.o \
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
        MatrixJ.o \
        mygaussj.o \
        normalForm.o


CCOBJS = Barnacle.o \
	BubbleSort.o \
        Distribution.o \
	ML.o \
        Matrix.o \
        Matrix_SVD.o \
	MatrixJ.o \
	MLJ.o \
        Vector.o \
        binomcoeff_recursive.o \
        cg.o \
        d_sign.o \
        derf.o \
        dlist.o \
	erf.o \
        gcd.o \
        menu.o \
        mod2pi.o \
        nearint.o \
        nexcom.o \
        parsef.o \
        rg.o \
        slist.o \
        strequals.o

OBJS = $(CCOBJS) $(MXOBJS)

SRCMXB = $(MXOBJS:.o=.cc)

SRC_CC = dlist.cc \
	slist.cc \
	Barnacle.cc \
	BubbleSort.cc \
	Distribution.cc \
	Matrix.cc \
	Matrix_SVD.cc \
	ML.cc \
	MatrixJ.cc \
	MLJ.cc \
	Vector.cc \
	erf.cc \
	menu.cc \
	nexcom.cc

SRC_C = binomcoeff_recursive.c \
        cg.c \
        d_sign.c \
        derf.c \
        gcd.c \
        mod2pi.c \
        nearint.c \
        parsef.c \
        rg.c \
        strequals.c


.cc.o:
	$(C++) -c $(INCLUDEDIR) $(C++FLAGS) $*.cc
.c.o:
	$(C) -c $(INCLUDEDIR) $(CFLAGS) $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libcxxlibr.a $(CCOBJS)
	$(MAKELIB) $(LIBDIR)/libmxyxptlk.a $(MXOBJS)
	$(RANLIB) $(LIBDIR)/libcxxlibr.a
	$(RANLIB) $(LIBDIR)/libmxyzptlk.a

clean:
	rm -f *.o *~*
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

depend:
	makedepend -- $(DEPEND_INC) -- $(SRCMXB) $(SRC_CC) $(SRC_C)


# DO NOT DELETE THIS LINE -- make depend depends on it.

