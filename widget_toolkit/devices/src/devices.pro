###################################################################
# 
# FILE:          devices.pro 
#
# DESCRIPTION:   qmake file for libdevices.a 
#                libdevices.dll
#                libdevices.lib
#
# AUTHOR:        Jean-Francois Ostiguy ostiguy@fnal.gov
#       
###################################################################

TEMPLATE	= lib
LANGUAGE	= C++

win32:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti
 unix:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti


unix:INCLUDEPATH += /usr/local/qt/include 
unix:INCLUDEPATH += /opt/chef/include 
unix:INCLUDEPATH += /usr/local/qt/include/qutexr
unix:INCLUDEPATH += .
unix:INCLUDEPATH += ../include 
unix:LIBS	 += -lqutexr -lqwt 


MSYS_TOPDIR       = /msys/1.0

win32:INCLUDEPATH += $$MSYS_TOPDIR/local/include
win32:INCLUDEPATH += $$MSYS_TOPDIR/local/include/qutexr
win32:LIBS	 += -lqutexr -lqwt 

HEADERS	+= ../include/devicetable.h

SOURCES	+= readdevice.cpp \
	   devicetable.cpp

FORMS	=  devicetablebase.ui


win32:UI_DIR       =  .\ui
win32:MOC_DIR      =  .\moc
win32:OBJECTS_DIR  =  .\obj

unix:UI_DIR       =  ./ui
unix:MOC_DIR      =  ./moc
unix:OBJECTS_DIR  =  ./obj


unix:QMAKE_LFLAGS_DEBUG     +=-Wl,--export-dynamic  -Wl,--rpath=/usr/local/lib -Wl,--rpath=/usr/local/qt/lib
unix:QMAKE_LFLAGS_RELEASE   +=-Wl,--export-dynamic  -Wl,--rpath=/usr/local/lib -Wl,--rpath=/usr/local/qt/lib


win32:target.path = ..\lib
win32:target.path = ..\lib
win32:INSTALLS += target

unix:target.path = ../lib
unix:target.path = ../lib
unix:INSTALLS += target

DESTDIR = .
