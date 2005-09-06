########################################################################
# 
# FILE:          devices.pro 
#
# AUTHOR:        Jean-Francois Ostiguy
#                Accelerator Division
#                Fermi National Accelerator Laboratory 
#                ostiguy@fnal.gov
#       
#######################################################################


include ( ../../chef-config/config.pri )

TEMPLATE	= lib
LANGUAGE	= C++

windows:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti
unix:CONFIG	+= qt warn_on debug thread dll       rtti exceptions 

HEADERS	+= include/devicetable.h

SOURCES	+= src/devicetable.cpp

FORMS	=  src/devicetablebase.ui


INCLUDEPATH += $$QUTEXR_INC

LIBS	 += -L$$(QUTEXR_LIBDIR) -lqutexr -lqwt 
LIBS	 += -Wl,-rpath,$${QUTEXR_LIBDIR} 


windows:LIBS	 += -L$${QUTEXR_LIBDIR} -lqutexr -lqwt 




