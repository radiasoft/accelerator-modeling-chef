.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I../include -I$(BASIC_TOOLKIT_INC)

EXE = Test  Test_sym  testQuad  testRoot

LIBS = -lintegrator -lbasic_toolkit -lm


.cc.o:
	$(C++) -c $(C++FLAGS) $(INCLUDEDIR) $*.cc
.c.o:
	$(C) -c $(CFLAGS) $(INCLUDEDIR) $*.c
.cc :
	$(C++) $(C++FLAGS) $(INCLUDEDIR) -o $* $*.cc \
        -L$(LIBDIR) $(LIBS)
.o :
	$(C++) $(C++FLAGS) -o $@ \
        $< -L$(LIBDIR) $(LIBS)



include ../../Make-config


targets: $(EXE)


depend:
	$(C++) $(INCLUDEDIR) $(DEPEND_FLAG) *.cc  > dependencies


include dependencies
