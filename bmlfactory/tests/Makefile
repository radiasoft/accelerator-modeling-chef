.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(FACTORY_INC) -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) $(STL_CONFIG)

OBJS = mad.tab.o \
       const_table.o \
       var_table.o \
       fb_allocator.o \
       beam_element.o \
       beamel_table.o \
       beam_line.o \
       bml_table.o \
       comment_arr.o \
       cfg_output.o \
       matrix.o \
       expression.o \
       mad.o \
       madparser.o \
       str_to_upper.o

SOURCES = $(OBJS:.o=.cc)

SUBLIBS =       -L/usr/local/ap/lib 
MYLIBS =        -lbmlfactory -lfilter \
                -lphysics_toolkit -lbeamline \
                -lphysics_toolkit -lbeamline \
                -lmxyzptlk -lbasic_toolkit -lfilter 
SYSLIBS =       -lglib -lsocket -lm -lnsl -lintl

LIBS    =       $(SUBLIBS) \
                $(MYLIBS) \
                $(SYSLIBS)

.cc.o:
	$(C++) $(C++FLAGS) $(INCLUDEDIR)  -c $*.cc
.cc :
	$(C++) $(C++FLAGS) $(INCLUDEDIR) -o $* $*.cc \
        -L$(LIBDIR) $(LIBS)
.c.o:
	$(C) $(CFLAGS) $(INCLUDEDIR) -c $*.c
.c :
	$(C) $(CFLAGS) $(INCLUDEDIR) -o $* $*.c \
        -L$(LIBDIR) $(LIBS)

include $(FNALROOT)/Make-config

all: mad2cfg test-factory

targets: mad2cfg test-factory

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
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc > dependencies


include dependencies
