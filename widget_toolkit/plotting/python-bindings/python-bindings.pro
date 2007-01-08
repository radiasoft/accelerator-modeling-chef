###################################################################
#
# python-bindings.pro
#
# python bindings for the chefplot widget
#
# Author: Jean-Francois Ostiguy
#         Accelerator Division
#         Fermi National Accelerator Laboratory 
#         ostiguy@fnal.gov
#
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++
TARGET          = pychefplot

unix:CONFIG	+= qt warn_on debug thread rtti exceptions dll

#
# Note: the relative path is broken under UNIX. Forward slashes do not seem to work, but win32 style backslashes are OK.
#
!include ( ../../../chef-config/config.pri ) {
        message( "Error: failed to find global configuration file config.pri" ) 
}


SOURCES	+= ./src/py-chefplot-module.cpp \
           ./src/py-chefplotmain.cpp \
           ./src/py-chefcurve.cpp \
           ./src/py-chefplotdata.cpp


INCLUDEPATH += ../chefplot/include
INCLUDEPATH += ../chefplot/src/ui
INCLUDEPATH +=  $${FNAL_INSTALL_TOP_DIR}/include/python-bindings

INCLUDEPATH += $${PYTHON_INC}

LIBS	+= -L$${FNAL_LIBDIR} -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -L$$GLIB_LIBDIR -lglib-2.0

unix:LIBS  +=  -Wl,-rpath,$${FNAL_LIBDIR}  -Wl,-rpath,$${GLIB_LIBDIR}

