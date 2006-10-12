## 
## This Makefile requires two environment variables
## be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
.SUFFIXES: .o .cpp .cc 

C++        = g++

INCS       = -I$(BOOST_INC) \
             -I$(INSTALLDIR)/include/gms \
             -I$(INSTALLDIR)/include/basic_toolkit \
             -I$(INSTALLDIR)/include/mxyzptlk \
             -I$(INSTALLDIR)/include/beamline \
             -I$(INSTALLDIR)/include/physics_toolkit \
             -I$(INSTALLDIR)/include

LIBS       = -L$(INSTALLDIR)/lib -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -Wl,-rpath,$(INSTALLDIR)/lib 
SYSLIBS    = -lglib -lm -lnsl -lintl

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS) $(SYSLIBS)

all: FODO_E MatrixCalcTest TuneAdjusterTest boosterv6 \
     demo_1 norm_A norm_B BBDemo_1 \
     ChromaticityAdjusterTest ChromaticityAdjusterTest_2 
