## 
## This Makefile requires two environment variables
## be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
.SUFFIXES: .o .cpp .cc 

C++        = g++

INCS       = -I$(BOOST_INC) \
             -I$(GLIB_INC) \
             -I$(GLIBCONFIG_INC) \
             -I$(INSTALLDIR)/include

## NOTE: Using a factory object in this directory breaks hierarchy.
## NOTE: beamline_iterators.cc does so.
## NOTE: The demo belongs at a higher level: e.g. bmlfactory.
LIBS       = -L$(INSTALLDIR)/lib -lbmlfactory -lbeamline -lmxyzptlk -lbasic_toolkit -Wl,-rpath,$(INSTALLDIR)/lib 
SYSLIBS    = -lglib -lm -lnsl -lintl

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS) $(SYSLIBS)

all: FODO_A  beamline_iterators

clean:	
	\rm *.o; \rm *Test; \rm hptest dfr evaltest pbtest survey concattest
