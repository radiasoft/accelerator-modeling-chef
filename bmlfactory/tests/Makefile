.SUFFIXES: .o .C .c .cc

INCLUDEDIR = -I$(FACTORY_INC) -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(BASIC_TOOLKIT_INC) $(STL_CONFIG)

SUBLIBS =       -L/usr/local/ap/lib 
MYLIBS =        -lbmlfactory \
                -lphysics_toolkit -lbeamline \
                -lphysics_toolkit -lbeamline \
                -lmxyzptlk -lbasic_toolkit 
SYSLIBS =       -lglib -lm -lnsl -lintl

LIBS    =       $(SUBLIBS) \
                $(MYLIBS) \
                $(SYSLIBS)

.cc.o:
	$(C++) -Dallocate=allocate__ -Ddeallocate=deallocate__ $(C++FLAGS) $(INCLUDEDIR)  -c $*.cc
.cc :
	$(C++) -Dallocate=allocate__ -Ddeallocate=deallocate__ $(C++FLAGS) $(INCLUDEDIR) -o $* $*.cc \
        -L$(LIBDIR) $(LIBS) -L/usr/local/ap/lib 
.c.o:
	$(C) -Dallocate=allocate__ -Ddeallocate=deallocate__ $(CFLAGS) $(INCLUDEDIR) -c $*.c
.c :
	$(C) -Dallocate=allocate__ -Ddeallocate=deallocate__ $(CFLAGS) $(INCLUDEDIR) -o $* $*.c \
        -L$(LIBDIR) $(LIBS) 

include $(FNALROOT)/Make-config

# targets: mad2cfg test-factory vlhcTest
targets: vlhcTest
