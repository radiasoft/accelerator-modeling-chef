TEMPLATE	= lib
LANGUAGE	= C++

unix:CONFIG	+= qt warn_on debug thread rtti exceptions dll
win32:CONFIG	+= qt warn_on       thread rtti exceptions staticlib

unix:LIBS	+= -lqwt -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -lglib-2.0



HEADERS	+= ../include/plot.h \
	../include/chefplot.h \
	../include/chefplotdata.h \
	../include/chefplotzoomer.h \
	../include/chefplotmain.h \
	../include/lego.h \
	../include/datadisplay.h \
	../include/TuneDialog.h

SOURCES	+= chefplot.cpp \
	chefplotmain.cpp \
	plot.cpp \
	chefplotdata.cpp \
	chefcurve.cpp \
	chefplotzoomer.cpp \
	lego.cpp \
	datadisplay.cpp \
	TuneDialog.cpp

FORMS	= chefplotmainbase.ui \
	datadisplaybase.ui \
	tunedialogbase.ui

IMAGES	= images/filenew \
	images/fileopen \
	images/filesave \
	images/print \
	images/undo \
	images/redo \
	images/editcut \
	images/editcopy \
	images/editpaste \
	images/searchfind

INCLUDEPATH += ..\include
win32:INCLUDEPATH += c:\boost\include\boost-1_32
win32:INCLUDEPATH += ..\..\..\chef-win32\include\beamline 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\bmlfactory 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\mxyzptlk 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\basic_toolkit 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\physics_toolkit 
win32:INCLUDEPATH += ..\..\..\..\..\local\include\glib-2.0 
win32:INCLUDEPATH += ..\..\..\..\..\local\lib\glib-2.0/include
win32:INCLUDEPATH += ..\..\..\widget_toolkit\include

target.path = ..\lib
INSTALLS += target

DESTDIR = ../lib

unix:QMAKE_LIBDIR_FLAGS += -L/opt/chef/lib 
unix:QMAKE_LIBDIR_FLAGS += -L/usr/local/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/lib -Wl,-rpath,/opt/chef/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/fnal-1st-order-proto-3/lib 

UI_DIR       =  .\ui
MOC_DIR      =  .\moc
OBJECTS_DIR  =  .\obj


QMAKE_CXXFLAGS_MT        += -pipe 
QMAKE_CXXFLAGS_MT_DBG    += -pipe 
QMAKE_CXXFLAGS_MT_DLL    += -pipe 
QMAKE_CXXFLAGS_MT_DLLDBG += -pipe 
QMAKE_CXXFLAGS_RELEASE   += -pipe 
QMAKE_CXXFLAGS_SHLIB     += -pipe 
