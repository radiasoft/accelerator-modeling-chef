#######################################################
## 
## File: Makefile for directory basic_toolkit/tests
## 
## This Makefile requires that two environment
## variables be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
## 
## It is purposely written using the minimal number
## of libraries and include directories.  In this
## way, CHEF's hierarchy is also tested.
## 
## Leo Michelotti
## Wednesday. February 13, 2008
## 
#######################################################


.SUFFIXES: .o .cc 

C++        = g++

INCS       = -I$(BOOST_INC) \
             -I$(INSTALLDIR)/include/gms \
             -I$(INSTALLDIR)/include/mxyzptlk \
             -I$(INSTALLDIR)/include/basic_toolkit \
             -I$(INSTALLDIR)/include

LIBS       = -L$(INSTALLDIR)/lib -lmxyzptlk -lbasic_toolkit \
             -Wl,-rpath,$(INSTALLDIR)/lib 

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS)
