########################################################################
#                                                                      #
# FILE:     chef.pro                                                   #
#                                                                      #
#                                                                      #
#                                                                      #
#                                                                      #
########################################################################


TEMPLATE	= app
LANGUAGE	= C++

unix:CONFIG	+= opengl x11 rtti stl thread exceptions 
#win32:CONFIG	+= opengl x11 rtti stl thread exceptions 

include ( ../chef-config/config.pri ) 


win32:RC_FILE = chefres.rc

INCLUDEPATH += ./include
INCLUDEPATH += ./licenses 

INCLUDEPATH +=  $${FNAL_INSTALL_TOP_DIR}/include/python-bindings

INCLUDEPATH += ../widget_toolkit/plotting/chefplot/include
INCLUDEPATH += ../widget_toolkit/plotting/chefplot/src/ui
INCLUDEPATH += ../widget_toolkit/interpreter/include
INCLUDEPATH += ../widget_toolkit/interpreter/src/ui
INCLUDEPATH += ../widget_toolkit/devices/include
INCLUDEPATH += ../widget_toolkit/devices/src/ui
INCLUDEPATH += ../widget_toolkit/databases/include
INCLUDEPATH += ../widget_toolkit/databases/src/ui


INCLUDEPATH += $$GLIBCONFIG_INC
INCLUDEPATH += $$GLIB_INC
INCLUDEPATH += $$QUTEXR_INC
INCLUDEPATH += $$PYTHON_INC


HEADERS	+= include/BeamlineBrowser.h \
	include/DataAlignWidget.h \
	include/PointEdit.h \
	include/QtMonitor.h \
	include/QueryDialog.h \
	include/EditDialog.h \
	include/RayTrace.h \
	include/SiteViewer.h \
	include/Tracker.h \
	include/TrbWidget.h \
	include/CHEFGUI.h \
	include/filters.h \
	include/messages.h \
	include/appworkspace.h \
	include/BmlSelectionDialog.h \
	include/LattFncData.h \
	include/ETFncData.h \
	include/MomentsFncData.h \
	include/DspnFncData.h \
	include/LBFncData.h \
	include/about.h \
        include/DistributionWidget.h \
	include/InitCondDialogLF.h \
	include/CF_Editor.h

SOURCES	+= src/BeamlineBrowser.cpp \
        src/DataAlignWidget.cpp \
  	src/PointEdit.cpp \
	src/QtMonitor.cpp \
        src/EditDialog.cpp \
	src/QueryDialog.cpp \
	src/RayTrace.cpp \
	src/SiteViewer.cpp \
	src/Tracker.cpp \
	src/TrbWidget.cpp \
	src/CHEFGUI.cpp \
	src/chefmain.cpp \
	src/builders.cpp \
	src/filters.cpp \
	src/messages.cpp \
	src/appworkspace.cpp \
	src/BmlSelectionDialog.cpp \
	src/LattFncData.cpp \
	src/ETFncData.cpp \
	src/MomentsFncData.cpp \
	src/DspnFncData.cpp \
	src/LBFncData.cpp \
	src/about.cpp \
        src/DistributionWidget.cpp \
	src/InitCondDialogLF.cpp \
	src/CF_Editor.cpp

FORMS	= src/chefguibase.ui \
	src/bmlselectiondialogbase.ui \
	src/aboutbase.ui \
	src/initconddialoglfbase.ui

IMAGES	= src/images/filenew \
	src/images/fileopen \
	src/images/filesave \
	src/images/print \
	src/images/undo \
	src/images/redo \
	src/images/editcut \
	src/images/editcopy \
	src/images/editpaste \
	src/images/searchfind \
	images/plot.xpm \
	images/run.xpm



QMAKE_LIBDIR  += /usr/local/ap/lib/python2.4
QMAKE_LIBDIR  += /usr/local/ap/lib



LIBS    += -L$${CHEF_LIBDIR} -linterpreter -ldevices -ldatabases -lpychefplot -lchefplot
LIBS    += -lpybmlfactory -lpyphysics_toolkit -lpybeamline -lpymxyzptlk -lpybasic_toolkit 
LIBS    += -L$${FNAL_LIBDIR} -lphysics_toolkit -lbmlfactory -lbeamline -lmxyzptlk -lbasic_toolkit 
LIBS    += -lqassistantclient
LIBS    += -lqutexr
LIBS    += -lqwt
LIBS    += -lglut
LIBS    += -L$${GLIB_DIR} -lglib-2.0   

unix:LIBS  += -Wl,-rpath,$${CHEF_LIBDIR} -Wl,-rpath,$${FNAL_LIBDIR} -Wl,-rpath,$${GLIB_LIBDIR} -Wl,-rpath,$${BOOST_LIBDIR} 

DESTDIR = .

