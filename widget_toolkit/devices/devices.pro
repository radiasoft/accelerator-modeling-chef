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


include ( "../config.pri" )

TEMPLATE	= lib
LANGUAGE	= C++

windows:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti
   unix:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti



HEADERS	+= include/devicetable.h

SOURCES	+= src/readdevice.cpp \
	   src/devicetable.cpp

FORMS	=  src/devicetablebase.ui


INCLUDEPATH += $$QUTEXR_INC

unix:LIBS	 += -L$$QUTEXR_LIBDIR -lqutexr -lqwt 
windows:LIBS	 += -L$$QUTEXR_LIBDIR -lqutexr -lqwt 


DESTDIR = .

windows:INSTALLS += target
unix:INSTALLS    += target

