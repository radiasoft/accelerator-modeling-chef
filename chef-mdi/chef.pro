#######################################################################
#                                                                      #
# FILE:     chef.pro                                                   #
#                                                                      #
########################################################################

TEMPLATE	= app
LANGUAGE	= C++

unix:CONFIG	+= qt debug opengl x11 rtti stl thread exceptions 
#win32:CONFIG	+= opengl x11 rtti stl thread exceptions 

include ( ../chef-config/config.pri ) 


win32:RC_FILE = ./src/chefres.rc

INCLUDEPATH += ./include
INCLUDEPATH += ./licenses 

INCLUDEPATH +=  $${FNAL_INSTALL_TOP_DIR}/include/python-bindings
INCLUDEPATH +=  $${FNAL_INSTALL_TOP_DIR}/include/parsers/xsif/tree-container

INCLUDEPATH += ../widget_toolkit/plotting/chefplot/include
INCLUDEPATH += ../widget_toolkit/plotting/chefplot/src/ui
INCLUDEPATH += ../widget_toolkit/interpreter/include
INCLUDEPATH += ../widget_toolkit/interpreter/src/ui
INCLUDEPATH += ../widget_toolkit/devices/include
INCLUDEPATH += ../widget_toolkit/devices/src/ui
INCLUDEPATH += ../widget_toolkit/databases/include
INCLUDEPATH += ../widget_toolkit/databases/src/ui
INCLUDEPATH += ../widget_toolkit/tracking/include
INCLUDEPATH += ../widget_toolkit/viewing/include
INCLUDEPATH += ../widget_toolkit/fncdata/include
INCLUDEPATH += ../widget_toolkit/dialogs/include

INCLUDEPATH += $$GLIBCONFIG_INC
INCLUDEPATH += $$GLIB_INC
INCLUDEPATH += $$QUTEXR_INC
INCLUDEPATH += $$PYTHON_INC


HEADERS	+= include/BmlSelectionDialog.h \
	include/CF_Editor.h \
	include/CHEFGUI.h \
	include/InitCondDialogLF.h \
	include/about.h \
	include/appworkspace.h \
	include/filters.h \
	include/messages.h \
	include/CommandTwiss.h \
	include/CommandPropagateTwiss.h \
	include/CommandMoments.h \
	include/CommandPropagateMoments.h \
	include/CommandEdwardsTeng.h \
	include/CommandEigenmodes.h \
	include/CommandDispersion.h \
	include/CommandPropagateDispersion.h

SOURCES	+= src/BmlSelectionDialog.cpp \
	src/CF_Editor.cpp \
	src/CHEFGUI.cpp \
	src/InitCondDialogLF.cpp \
	src/about.cpp \
	src/appworkspace.cpp \
	src/builders.cpp \
	src/chefmain.cpp \
	src/filters.cpp \
	src/messages.cpp \
	src/CF_rbendFinder_quad.cpp \
	src/RollAccumulator.cpp \
	src/CommandTwiss.cpp \
	src/CommandPropagateTwiss.cpp \
	src/CommandMoments.cpp \
	src/CommandPropagateMoments.cpp \
	src/CommandEdwardsTeng.cpp \
	src/CommandEigenmodes.cpp \
	src/CommandDispersion.cpp \
	src/CommandPropagateDispersion.cpp

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

LIBS    += -L$${CHEF_LIBDIR} -linterpreter -ldevices -ldatabases -lpychefplot -lchefplot -ltunespace
LIBS    += -L$${CHEF_LIBDIR} -lviewing -ltracking -lfncdata -ldialogs
LIBS    += -L$${FNAL_LIBDIR} -lpybmlfactory -lpyphysics_toolkit -lpybeamline -lpymxyzptlk -lpybasic_toolkit 
LIBS    += -L$${FNAL_LIBDIR} -lphysics_toolkit -lbmlfactory -lxsifparser -lbeamline -lmxyzptlk -lbasic_toolkit 
LIBS    += -lqassistantclient
LIBS    += -lqutexr
LIBS    += -lqwt
#LIBS    += -lqwt4
LIBS    += -lglut
LIBS    += -L$${BOOST_LIBDIR} -lboost_regex 
LIBS    += -L$${GLIB_DIR} -lglib-2.0  

unix:LIBS  += -Wl,-rpath,$${CHEF_LIBDIR} -Wl,-rpath,$${FNAL_LIBDIR} -Wl,-rpath,$${GLIB_LIBDIR} -Wl,-rpath,$${BOOST_LIBDIR} -Wl,-rpath,/usr/local/lib

DESTDIR = .

