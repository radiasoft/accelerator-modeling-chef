########################################################################
# 
# FILE:          interpreter.pro 
#
# AUTHOR:        Jean-Francois Ostiguy
#                Accelerator Division
#                Fermi National Accelerator Laboratory 
#                ostiguy@fnal.gov
#       
#######################################################################

include( "../config.pri" )

TEMPLATE	= lib
LANGUAGE	= C++

   unix:CONFIG	+= qt warn_on debug thread rtti exception dll
windows:CONFIG	+= qt warn_on debug thread rtti exceptions staticlib



HEADERS	+= ./include/ioredirector.h \
 	   ./include/qpychef.h \
           ./include/interpreter.h

SOURCES	+= ./src/qpychef.cpp \
           ./src/interpreter.cpp \
           ./src/ioredirector.cpp \
           ./src/py-ioredirector.cpp \
	   ./src/python-specific/2.3/testcomplete.c 

FORMS	=  ./src/qpychefbase.ui

INCLUDEPATH += $$PYTHON_INC

unix:LIBS += -L$$CHEF_LIBDIR -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
unix:LIBS += -lqwt 
#unix:LIBS += /usr/lib/python2.3/config/libpython2.3.a -L/usr/lib -lboost_python-gcc-mt-1_32 
unix:LIBS += /usr/lib/python2.3/config/libpython2.3.a -L/usr/lib -lboost_python-mt 
unix:LIBS += -L$$FNAL_LIBDIR-lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 

windows:LIBS += -L/opt/chef/lib -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
windows:LIBS += -lqwt 
windows:LIBS += /usr/local/lib/python2.3/config/libpython2.3.a -L/usr/local/lib -lboost_python-gcc-mt-1_32 
windows:LIBS += -L/opt/chef/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 


unix:QMAKE_LFLAGS     +=-Wl,--export-dynamic  -Wl,--rpath,$$FNAL_LIBDIR   -Wl,--rpath,$$CHEF_LIBDIR 

INSTALLS += target

DESTDIR = .
