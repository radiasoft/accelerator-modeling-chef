## 
## This Makefile requires two environment variables
## be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
.SUFFIXES: .o .cpp .cc 

C++        = g++

INSTALLDIR = ../../RUNTIME
INCS       = -I/usr/local/ap/include/boost-1_33 \
             -I$(INSTALLDIR)/include/gms \
             -I$(INSTALLDIR)/include/mxyzptlk \
             -I$(INSTALLDIR)/include/basic_toolkit \
             -I$(INSTALLDIR)/include

LIBS       = -L$(INSTALLDIR)/lib -lmxyzptlk -lbasic_toolkit -Wl,-rpath,$(INSTALLDIR)/lib 
SYSLIBS    = -lglib -lm -lnsl -lintl

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS) $(SYSLIBS)

all: hptest dfr evaltest orthogonalTest g5 envTest \
     pbtest survey CMapTest SVDTest concattest coordTest \
     Inverse_B_Test Inverse_C_Test Inverse_D_Test Inverse_E_Test \
     Lie_A_Test Lie_B_Test Lie_C_Test Lie_D_Test Lie_E_Test \
     Lie_F_Test Lie_G_Test Lie_H_Test Lie_I_Test Lie_J_Test \
     Limit_A_Test Limit_B_Test Limit_C_Test Limit_D_Test \
     Limit_E_Test Limit_F_Test Limit_G_Test Limit_H_Test

clean:	
	\rm *.o; \rm *Test; \rm hptest dfr evaltest pbtest survey concattest
