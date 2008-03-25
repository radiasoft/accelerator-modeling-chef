## 
## This Makefile requires two environment variables
## be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
.SUFFIXES: .o .cpp .cc 

C++        = g++

INCS       = -I$(BOOST_INC) \
             -I$(INSTALLDIR)/include

LIBS       = -L$(INSTALLDIR)/lib -lintegrator -lbasic_toolkit -Wl,-rpath,$(INSTALLDIR)/lib 
SYSLIBS    = -lglib -lm -lnsl -lintl

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS) $(SYSLIBS)

all: Test Test_sym
