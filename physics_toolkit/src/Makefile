.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) -I$(PHYSICS_TOOLKIT_INC)

OBJS = ConvertToTunes.o \
	EdwardsTeng.o \
	FindCovariance.o \
	FPSolver.o \
	JacobianVisitor.o \
	normalForm.o \
	particleVisitor.o

SOURCES = $(OBJS:.o=.cc)

.cc.o:
	$(C++) $(C++FLAGS) $(INCLUDEDIR)  -c $*.cc
.c.o:
	$(C) $(CFLAGS) $(INCLUDEDIR) -c $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libphysics_toolkit.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libphysics_toolkit.a

clean:
	rm -f *.o *~*
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

real-clean: clean
	rm -f dependencies
	touch dependencies


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies

