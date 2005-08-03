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
win32:CONFIG	+= opengl x11 rtti stl thread exceptions 

win32:RC_FILE = chefres.rc

#DEFINES	+= QT_DEBUG


INCLUDEPATH	+= /usr/local/include/boost-1_32
INCLUDEPATH	+= /usr/local/qt/include
INCLUDEPATH	+= /usr/local/qt/include/qutexr

INCLUDEPATH	+= ../include 
INCLUDEPATH	+= ../licenses 

INCLUDEPATH	+= ../../acnet-devices/include
INCLUDEPATH	+= ../../acnet-devices/src/ui


win32:INCLUDEPATH += ..\include
win32:INCLUDEPATH += ..\..\..\chef-win32\include\basic_toolkit
win32:INCLUDEPATH += ..\..\..\chef-win32\include\bmlfactory       
win32:INCLUDEPATH += ..\..\..\chef-win32\include\mxyzptlk
win32:INCLUDEPATH += ..\..\..\chef-win32\include\beamline
win32:INCLUDEPATH += ..\..\..\chef-win32\include\integrator
win32:INCLUDEPATH += ..\..\..\chef-win32\include\physics_toolkit
win32:INCLUDEPATH += ..\..\..\chef-win32\include\widget_toolkit
win32:INCLUDEPATH += ..\..\widget_toolkit\include

INCLUDEPATH += ../include
INCLUDEPATH += $(INSTALLDIR)/include/basic_toolkit
INCLUDEPATH += $(INSTALLDIR)/include/bmlfactory
INCLUDEPATH += $(INSTALLDIR)/include/gms
INCLUDEPATH += $(INSTALLDIR)/include/mxyzptlk
INCLUDEPATH += $(INSTALLDIR)/include/beamline
INCLUDEPATH += $(INSTALLDIR)/include/integrator
INCLUDEPATH += $(INSTALLDIR)/include/physics_toolkit

INCLUDEPATH += ../../widget_toolkit/chefplot/include
INCLUDEPATH += ../../widget_toolkit/chefplot/src/ui
INCLUDEPATH += ../../widget_toolkit/interpreter/include
INCLUDEPATH += ../../widget_toolkit/interpreter/src/ui
INCLUDEPATH += ../../widget_toolkit/devices/include
INCLUDEPATH += ../../widget_toolkit/devices/src/ui

INCLUDEPATH += /usr/include/glib-2.0
INCLUDEPATH += /usr/lib/glib-2.0/include

#INCLUDEPATH += /msys/1.0/local/include/glib-2.0
#INCLUDEPATH += /msys/1.0/local/lib/glib-2.0/include
#INCLUDEPATH += /msys/1.0/local/include/qutexr


win32:INCLUDEPATH     += /python23/include
win32:INCLUDEPATH     += /msys/1.0/local/include/qutexr
win32:INCLUDEPATH     += /msys/1.0/local/include

unix:INCLUDEPATH     += /usr/local/include/python2.3
#unix:INCLUDEPATH     += /msys/1.0/local/include/qutexr
#unix:INCLUDEPATH     += /msys/1.0/local/include


unix:LIBS	+= -lqwt -lqt-mt -lqassistantclient -lqscintilla


HEADERS	+= ../include/BeamlineBrowser.h \
	../include/PointEdit.h \
	../include/QtMonitor.h \
	../include/QueryDialog.h \
	../include/EditDialog.h \
	../include/RayTrace.h \
	../include/SiteViewer.h \
	../include/Tracker.h \
	../include/TrbWidget.h \
	../include/CHEFGUI.h \
	../include/filters.h \
	../include/messages.h \
	../include/appworkspace.h \
	../include/BmlSelectionDialog.h \
	../include/LattFncData.h \
	../include/ETFncData.h \
	../include/MomentsFncData.h \
	../include/DspnFncData.h \
	../include/LBFncData.h \
	../include/about.h \
        ../include/DistributionWidget.h \
	../include/InitCondDialogLF.h \
	../include/CF_Editor.h

SOURCES	+= BeamlineBrowser.cpp \
	PointEdit.cpp \
	QtMonitor.cpp \
        EditDialog.cpp \
	QueryDialog.cpp \
	RayTrace.cpp \
	SiteViewer.cpp \
	Tracker.cpp \
	TrbWidget.cpp \
	CHEFGUI.cpp \
	chefmain.cpp \
	builders.cpp \
	filters.cpp \
	messages.cpp \
	appworkspace.cpp \
	BmlSelectionDialog.cpp \
	LattFncData.cpp \
	ETFncData.cpp \
	MomentsFncData.cpp \
	DspnFncData.cpp \
	LBFncData.cpp \
	about.cpp \
        DistributionWidget.cpp \
	InitCondDialogLF.cpp \
	CF_Editor.cpp

FORMS	= chefguibase.ui \
	bmlselectiondialogbase.ui \
	aboutbase.ui \
	initconddialoglfbase.ui

IMAGES	= images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/undo \
	images/redo \
	images/editcut \
	images/editcopy \
	images/editpaste \
	images/searchfind \
	../images/plot.xpm \
	../images/run.xpm



# libutil is needed for openpty/forkpty (glibc) 
unix:LIBS	+= -lutil






UI_DIR      = .\ui
MOC_DIR     = .\moc
OBJECTS_DIR = .\obj


DESTDIR = .
LIBS += -lqassistantclient

LIBS += -L$(INSTALLDIR)/lib
LIBS += -linterpreter -lchefplot -ldevices 
LIBS += -lpychef -lpychefplot -lpybmlfactory -lpyphysics_toolkit -lpybeamline -lpymxyzptlk -lpybasic_toolkit 
LIBS += -lphysics_toolkit -lbmlfactory -lbeamline -lmxyzptlk -lbasic_toolkit 
LIBS += -lqutexr
LIBS += -lqwt
LIBS += -lglut
LIBS += -lglib-2.0   
##LIBS += -lutil

#QMAKE_LIBDIR += /msys/1.0/home/Administrator/chef-win32/lib
#QMAKE_LIBDIR += /msys/1.0/local/lib
QMAKE_LIBDIR += /usr/local/lib/python2.3
QMAKE_LIBDIR += /usr/local/lib

QMAKE_LDFLAGS += -Wl,--rpath,$(INSTALLDIR)/lib
#QMAKE_LDFLAGS += -Wl,-rpath,/opt/chef/lib
