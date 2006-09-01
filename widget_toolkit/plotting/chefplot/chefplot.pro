###################################################################
#
# chefplot.pro
#
# Author: Jean-Francois Ostiguy
#         Accelerator Division
#         Fermi National Accelerator Laboratory 
#         ostiguy@fnal.gov
#
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++

#
# Note: the relative path is broken under UNIX. Forward slashes do not seem to work, but win32 style backslashes are OK.
#
!include ( ../../../chef-config/config.pri ) {
        message( "Error: failed to find global configuration file config.pri" ) 
}

unix:CONFIG	+= qt warn_on debug thread rtti exceptions dll

HEADERS	+= ./include/plot.h \
	   ./include/chefplot.h \
	   ./include/chefplotdata.h \
	   ./include/chefplotzoomer.h \
	   ./include/chefplotmain.h \
	   ./include/lego.h \
	   ./include/datadisplay.h

SOURCES	+= ./src/chefplot.cpp \
	   ./src/chefplotmain.cpp \
	   ./src/plot.cpp \
	   ./src/chefplotdata.cpp \
	   ./src/chefcurve.cpp \
	   ./src/chefplotzoomer.cpp \
	   ./src/lego.cpp \
	   ./src/datadisplay.cpp

FORMS	= ./src/chefplotmainbase.ui \
	  ./src/datadisplaybase.ui \
	  ./src/tunedialogbase.ui

IMAGES	= ./src/images/filenew \
	  ./src/images/fileopen \
	  ./src/images/filesave \
	  ./src/images/print \
	  ./src/images/undo \
	  ./src/images/redo \
	  ./src/images/editcut \
	  ./src/images/editcopy \
	  ./src/images/editpaste \
	  ./src/images/searchfind


INCLUDEPATH += ../tunespace/include

LIBS	   +=  -L$${FNAL_LIBDIR} -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -L$${GLIB_LIBDIR} -lglib-2.0
LIBS       +=  -lqwt

unix:LIBS  +=  -Wl,--rpath,$${FNAL_LIBDIR} -Wl,--rpath,$${GLIB_LIBDIR}

