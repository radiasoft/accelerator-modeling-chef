.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC)

OBJS =  MADquad.o \
	MADsbend.o \
	MADrbend.o \
	MADJetQuad.o

SOURCES = MADquad.cc \
	MADsbend.cc \
	MADrbend.cc \
	MADJetQuad.cc

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

real-clean: clean
	rm -f dependencies
	touch dependencies


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies
