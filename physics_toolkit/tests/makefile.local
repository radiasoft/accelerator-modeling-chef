#######################################################
## 
## File: Makefile for directory physics_toolkit/tests
## 
## This Makefile requires that two environment
## variables be predefined: BOOST_INC  and INSTALLDIR
## See the chef-config/config.pri.* files 
## for examples.
## 
## It is purposely written using the minimal number
## of libraries and include directories.  In this
## way, CHEF's hierarchy is also tested.
## 
## Leo Michelotti
## Saturday. February 16, 2008
## 
#######################################################

.SUFFIXES: .o .cpp .cc 

C++        = g++

INCS       = -I$(BOOST_INC) \
             -I$(INSTALLDIR)/include

LIBS       = -L$(INSTALLDIR)/lib \
             -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit \
             -Wl,-rpath,$(INSTALLDIR)/lib 

.cc.o:
	$(C++) $(C++FLAGS) $(INCS) -c $*.cc
.o :
	$(C++) $(C++FLAGS) -o $@ $< $(LIBS) $(SYSLIBS)
.cc :
	$(C++) $(C++FLAGS) $(INCS) -o $@ $< $(LIBS)
