MYINCLUDEDIR = -I$(FNALROOT)/integrator/include \
               -I$(FNALROOT)/mxb/include \
               -I$(FNALROOT)/filter/include \
               -I$(FNALROOT)/socket/include
MYLIBDIR = -L$(FNALROOT)/lib/ultra
MYLIBRARIES = -lMachine -lfilter -lBEAMLINE -lPHYS -lBEAMLINE -lmxb -lcxxlibr -lsocket++ -lsocket -lintegrator


# ...... Sun-C++
C++   = CC 
# C++   = CC  -g -sb
C     = cc 

.cc.o:
	$(C++) -c $(MYINCLUDEDIR) $*.cc

.cc :
	$(C++) -o $@ \
        -I. $(MYINCLUDEDIR) \
        $< \
        $(MYLIBDIR) $(MYLIBRARIES)  -lcomplex -lm

.o :
	$(C++) -o $@ \
        $< $(MYLIBDIR) $(MYLIBRARIES)  -lcomplex -lm
