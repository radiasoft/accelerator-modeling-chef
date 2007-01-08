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

include( ../../chef-config/config.pri )

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
	   ./src/python-specific/$$PYTHON_VERSION/testcomplete.c 

FORMS	=  ./src/qpychefbase.ui

INCLUDEPATH += $$PYTHON_INC

LIBS += -L../plotting/python-bindings/lib -lpychefplot -L$$CHEF_LIBDIR -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit  
LIBS += -L$$FNAL_LIBDIR -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 
##LIBS += -L$$PYTHON_LIBDIR -lpython$$PYTHON_VERSION
LIBS +=  $${PYTHON_LIBDIR}/libpython$${PYTHON_VERSION}.a 
#LIBS += -L$${BOOST_LIBDIR} -lboost_python-gcc-mt
LIBS += -L$${BOOST_LIBDIR} -lboost_python-mt

unix:LIBS  +=  -Wl,--export-dynamic -Wl,-rpath,$${CHEF_LIBDIR} -Wl,-rpath,$${FNAL_LIBDIR} -Wl,-rpath,$${GLIB_LIBDIR} -Wl,-rpath,$${BOOST_LIBDIR} 

windows:LIBS += -L/opt/chef/lib -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
windows:LIBS += /usr/local/lib/python2.3/config/libpython2.3.a -L/usr/local/lib -lboost_python-gcc-mt-1_32 
windows:LIBS += -L/opt/chef/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 
