.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC)

DEPEND_INC = $(INCLUDEDIR) -I$(COMPILER_DEPEND)


OBJS =  MADquad.o MADsbend.o MADrbend.o MADJetQuad.o

SOURCES = $(OBJS:.o=.cc)

include $(FNALROOT)/Make-config

.cc.o:
	$(C++) $(C++FLAGS) $(INCLUDEDIR) -c $*.cc
.c.o:
	$(C) $(CFLAGS) $(INCLUDEDIR) -c $*.c

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

