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
             -I$(INSTALLDIR)/include/mxyzptlk \
             -I$(INSTALLDIR)/include/basic_toolkit \
             -I$(INSTALLDIR)/include

## 
## THIS DOESN'T WORK, BUT SHOULD!!
## 
LIBS       = -L$(INSTALLDIR)/lib -lmxyzptlk -lbasic_toolkit \
             -Wl,-rpath,$(INSTALLDIR)/lib 

## 
## THIS DOES WORK, BUT USAGE OF libphysics_toolkit
## AND libbeamline SHOULD NOT BE NECESSARY!!
## 
LIBS       = -L$(INSTALLDIR)/lib \
             -lphysics_toolkit -lbeamline \
             -lmxyzptlk -lbasic_toolkit \
             -Wl,-rpath,$(INSTALLDIR)/lib 

SYSLIBS    = -lm -lnsl

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS) $(SYSLIBS)

all: concattest coordTest dfr envTest evaltest g5 \
     Hermite hptest orthogonalTest pbtest survey SVDTest \
     Inverse_B_Test Inverse_C_Test Inverse_D_Test Inverse_E_Test CMapTest \
     Limit_A_Test Limit_B_Test Limit_C_Test Limit_D_Test Limit_E_Test \
     Limit_F_Test Limit_G_Test Limit_H_Test \
     Lie_A_Test Lie_B_Test Lie_C_Test Lie_D_Test Lie_E_Test \
     Lie_F_Test Lie_G_Test Lie_H_Test Lie_I_Test Lie_J_Test


clean:	
	\rm *.o; \rm *Test; \rm hptest dfr evaltest pbtest survey concattest

