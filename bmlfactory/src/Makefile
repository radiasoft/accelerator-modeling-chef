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
       bmlfactory.o \
       comment_arr.o \
       cfg_output.o \
       matrix.o \
       expression.o \
       mad.o \
       madparser.o \
       str_to_upper.o \
       yywrap.o

SOURCES = $(OBJS:.o=.cc)

.cc.o:
	$(C++) -Dallocate=allocate__ -Ddeallocate=deallocate__ \
        $(C++FLAGS) -Wall -Wcast-align -g \
        $(INCLUDEDIR)  -c $*.cc
.c.o:
	$(C) -Dallocate=allocate__ -Ddeallocate=deallocate__ \
        $(CFLAGS) -Wall -Wcast-align -g \
        $(INCLUDEDIR) -c $*.c

include $(FNALROOT)/Make-config

all: lib

targets: lib

lib: $(OBJS)
	$(MAKELIB) $(LIBDIR)/libbmlfactory.a $(OBJS)
	$(RANLIB) $(LIBDIR)/libbmlfactory.a

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
