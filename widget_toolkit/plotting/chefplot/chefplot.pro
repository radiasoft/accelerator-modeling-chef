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

include ( "../config.pri" )

unix:CONFIG	+= qt warn_on debug thread rtti exceptions dll

unix:LIBS	+= -lqwt -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -lglib-2.0


HEADERS	+= ./include/plot.h \
	   ./include/chefplot.h \
	   ./include/chefplotdata.h \
	   ./include/chefplotzoomer.h \
	   ./include/chefplotmain.h \
	   ./include/lego.h \
	   ./include/datadisplay.h \
	   ./include/TuneDialog.h

SOURCES	+= ./src/chefplot.cpp \
	   ./src/chefplotmain.cpp \
	   ./src/plot.cpp \
	   ./src/chefplotdata.cpp \
	   ./src/chefcurve.cpp \
	   ./src/chefplotzoomer.cpp \
	   ./src/lego.cpp \
	   ./src/datadisplay.cpp \
	   ./src/TuneDialog.cpp

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


INSTALLS += target

DESTDIR = .

unix:QMAKE_LIBDIR_FLAGS += -L$$CHEF_LIBDIR 
unix:QMAKE_LIBDIR_FLAGS += -L$$FNAL_LIBDIR
unix:QMAKE_LIBDIR_FLAGS += -L$$GLIB_LIBDIR

unix:QMAKE_LFLAGS += -Wl,--rpath,$$CHEF_LIBDIR 
unix:QMAKE_LFLAGS += -Wl,--rpath,$$FNAL_LIBDIR 
unix:QMAKE_LFLAGS += -Wl,--rpath,$$GLIB_LIBDIR 



