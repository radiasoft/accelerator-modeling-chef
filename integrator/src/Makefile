.SUFFIXES: .o .C .c .cc

INCLUDEDIR =  -I$(INTEGRATOR_INC) -I$(BASIC_TOOLKIT_INC)

OBJS = ODE.o \
       Oscillator.o \
       DriftMap.o \
       KickMap.o \
       Integrator.o \
       RungeKutta.o \
       Symplectic.o 

.cc.o:
	$(C++) -c $(INCLUDEDIR) $(C++FLAGS) $*.cc
.c.o:
	$(C) -c $(INCLUDEDIR) $(CFLAGS) $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libintegrator.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libintegrator.a

clean:
	rm -f *.o *~*
	rm -f  ../include/*~*
	rm -f  ../*~*
	rm -rf .sb

real-clean: clean
	rm -f dependencies
	touch dependencies


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc *.c > dependencies


include dependencies
