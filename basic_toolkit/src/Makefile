.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BASIC_TOOLKIT_INC)

DEPEND_INC = $(INCLUDEDIR) -I$(COMPILER_DEPEND)



OBJS = Barnacle.o \
	BubbleSort.o \
        Distribution.o \
	IntArray.o \
	LookupTable.o \
	ML.o \
        Matrix.o \
        Matrix_SVD.o \
        Vector.o \
        binomcoeff_recursive.o \
        cg.o \
        d_sign.o \
        dlist.o \
	erf.o \
        gcd.o \
        menu.o \
        mod2pi.o \
	mygaussj.o \
        nearint.o \
        nexcom.o \
        OutputFormat.o \
        parsef.o \
        rg.o \
        slist.o \
        strequals.o


SOURCES = $(OBJS:.o=.cc)


.cc.o:
	$(C++) -c $(INCLUDEDIR) $(C++FLAGS) $*.cc
.c.o:
	$(C) -c $(INCLUDEDIR) $(CFLAGS) $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libbasic_toolkit.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libbasic_toolkit.a

clean:
	rm -f *.o *~*
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

depend:
	makedepend -- $(DEPEND_INC) -- $(SOURCES)


# DO NOT DELETE THIS LINE -- make depend depends on it.
