TEMPLATE	= lib
LANGUAGE	= C++

###CONFIG	+= qt warn_on debug thread rtti exception staticlib
CONFIG	+= qt warn_on debug thread rtti exceptions staticlib

unix:LIBS += -L/opt/chef/lib -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
unix:LIBS += -lqwt 
unix:LIBS += /usr/local/lib/python2.3/config/libpython2.3.a -L/usr/local/lib -lboost_python-gcc-mt-1_32 
unix:LIBS += -L/opt/chef/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 
###unix:LIBS += -L/usr/local/lib -lglib-2.0 
###unix:LIBS += -L/usr/local/lib -lglib-2.0 -lutil

unix:INCLUDEPATH += . ../include 
unix:INCLUDEPATH += /usr/local/include/boost-1_32 
#unix:INCLUDEPATH += /usr/local/include/qwt 
unix:INCLUDEPATH += /usr/local/include/python2.3 
unix:INCLUDEPATH += /opt/chef/include/cxxutils 
unix:INCLUDEPATH += /opt/chef/include/beamline /opt/chef/include/bmlfactory /opt/chef/include/mxyzptlk /opt/chef/include/basic_toolkit /opt/chef/include/physics_toolkit 
unix:INCLUDEPATH += /usr/include/glib-2.0 
unix:INCLUDEPATH += /usr/lib/glib-2.0/include 
unix:INCLUDEPATH += /usr/local/include/boost-1_32

win32:INCLUDEPATH += . ../include 
win32:INCLUDEPATH += /boost/include/boost-1_32 
win32:INCLUDEPATH += /python23/include 

win32:INCLUDEPATH += ..\..\..\chef-win32\include\beamline 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\bmlfactory 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\mxyzptlk 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\basic_toolkit 
win32:INCLUDEPATH += ..\..\..\chef-win32\include\physics_toolkit 
win32:INCLUDEPATH += ..\..\..\..\..\local\include\glib-2.0 
win32:INCLUDEPATH += ..\..\..\..\..\local\lib\glib-2.0\include 

HEADERS	+= ../include/ioredirector.h \
 	../include/qpychef.h \
        ../include/interpreter.h

SOURCES	+= qpychef.cpp interpreter.cpp ioredirector.cpp py-ioredirector.cpp \
	./python-specific/testcomplete.c 

FORMS	= qpychefbase.ui

unix {
  UI_DIR =      ui
  MOC_DIR =     moc
  OBJECTS_DIR = obj
}

win32{

  UI_DIR      = .\ui
  MOC_DIR     = .\moc
  OBJECTS_DIR = .\obj
}

unix:QMAKE_LFLAGS_DEBUG     +=-Wl,--export-dynamic  -Wl,-rpath,/home/Administrator/chef-win32/lib -Wl,-rpath,/usr/local/lib  
unix:QMAKE_LFLAGS_RELEASE   +=-Wl,--export-dynamic  -Wl,-rpath,/home/Administrator/chef-win32/lib -Wl,-rpath,/usr/local/lib


target.path = /home/Administrator/chef-win32/lib
INSTALLS += target

DESTDIR = ../lib
