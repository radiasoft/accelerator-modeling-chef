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

none: 

clean:	
	\rm *.o; \rm *Test; \rm hptest dfr evaltest pbtest survey concattest

