.SUFFIXES: .o .C .c .cc

include $(FNALROOT)/Make-config

INCLUDEDIR = -I$(BEAMLINE_INC) -I$(MXYZPTLK_INC) -I$(FILTER_INC) \
             -I$(BASIC_TOOLKIT_INC) -I$(PHYSICS_TOOLKIT_INC)

INCLUDE = -I. $(INCLUDEDIR) 

MYLIBDIR = -L$(FNALROOT)/lib/sun 
MYLIBS = -lfilter -lphysics_toolkit -lbeamline -lmxyzptlk \
         -lbasic_toolkit -lfilter -lsocket++
SYBASELIBS = -lct -lcs -lcomn -lsybtcl -lsybintl -lblk
XLIBS = -L/usr/local/ap/X11R6/lib -L/usr/local/TCL-TK/lib -ltcl -ltk \
        -lX11 -lICE -lSM 
SYSLIBS = -lcomplex -lm -lsocket -lnsl -lintl

CC = cc
C++ = CC

CFLAGS   = 
C++FLAGS = 

.C.o:
	$(C++) $(C++FLAGS) $(INCLUDE) -c $*.C

.cc.o:
	$(C++) $(C++FLAGS) $(INCLUDE) -c $*.cc

.c.o:
	$(CC) $(CFLAGS) $(INCLUDE) -c $*.c


ALL = FODO_E boosterv6 norm_A norm_B normalFormTest 

all: $(ALL)

FODO_E: FODO_E.o
	$(C++) $(C++FLAGS) -o $@ $? \
               $(MYLIBDIR) $(MYLIBS) $(SYSLIBS)

boosterv6: boosterv6.o
	$(C++) $(C++FLAGS) -o $@ $? \
               $(MYLIBDIR) $(MYLIBS) $(SYSLIBS)

norm_A: norm_A.o
	$(C++) $(C++FLAGS) -o $@ $?\
               $(MYLIBDIR) $(MYLIBS) $(SYSLIBS)

norm_B: norm_B.o
	$(C++) $(C++FLAGS) -o $@ $?\
               $(MYLIBDIR) $(MYLIBS) $(SYSLIBS)

normalFormTest: normalFormTest.o
	$(C++) $(C++FLAGS) -o $@ $? \
               $(MYLIBDIR) $(MYLIBS) $(SYSLIBS)

clean:
	rm -f *.o
	rm -rf .sb
	rm -f *~*
	rm -f $(ALL)

depend:
	makedepend -- $(DEPEND_INC) -- $(SOURCES)


# DO NOT DELETE THIS LINE -- make depend depends on it.









