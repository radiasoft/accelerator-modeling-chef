########################################################################
# 
# FILE:          devices.pro 
#
# AUTHOR:        Jean-Francois Ostiguy
#                Accelerator Division
#                Fermi National Accelerator Laboratory 
#                ostiguy@fnal.gov
#       
#######################################################################
#
# Please define the following
#
########################################################################
   unix:CHEF_INSTALL_TOP_DIR  = /usr/local/ap/FNAL  
windows:CHEF_INSTALL_TOP_DIR  = ..\..\..\..\chef-win32   

   unix:FNAL_INSTALL_TOP_DIR  = /usr/local/ap/FNAL  
windows:FNAL_INSTALL_TOP_DIR  = ..\..\..\..\chef-win32   

   unix:BOOST_INSTALL_TOP_DIR = /usr/local/ap 
windows:BOOST_INSTALL_TOP_DIR = c:\boost

   unix:QUTEXR_INSTALL_TOP_DIR =  /usr/local/ap/qt3/include
windows:QUTEXR_INSTALL_TOP_DIR =  c:\Qt\3.3.3\include 

########################################################################
# THERE SHOULD BE NO NEED TO CHANGE ANYTHING BELOW THIS LINE !
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++

windows:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti
   unix:CONFIG	+= qt debug warn_on thread staticlib exceptions rtti



unix:INCLUDEPATH += .
unix:INCLUDEPATH += ../include 
unix:INCLUDEPATH += $$join(QUTEXR_INSTALL_TOP_DIR, "/qutexr" ,, /qutexr) 

windows:INCLUDEPATH += .
windows:INCLUDEPATH += ../include 
windows:INCLUDEPATH += $$join(QUTEXR_INSTALL_TOP_DIR, "\qutexr" ,,   \qutexr       )


unix:LIBS	 += -lqutexr -lqwt 
windows:LIBS	 += -lqutexr -lqwt 

HEADERS	+= ../include/devicetable.h

SOURCES	+= readdevice.cpp \
	   devicetable.cpp

FORMS	=  devicetablebase.ui


windows:UI_DIR       =  .\ui
windows:MOC_DIR      =  .\moc
windows:OBJECTS_DIR  =  .\obj

unix:UI_DIR       =  ./ui
unix:MOC_DIR      =  ./moc
unix:OBJECTS_DIR  =  ./obj


unix:QMAKE_LFLAGS_DEBUG     +=-Wl,--export-dynamic  -Wl,--rpath=/usr/local/lib -Wl,--rpath=/usr/local/qt/lib
unix:QMAKE_LFLAGS_RELEASE   +=-Wl,--export-dynamic  -Wl,--rpath=/usr/local/lib -Wl,--rpath=/usr/local/qt/lib


windows:target.path = ..\lib
windows:target.path = ..\lib
windows:INSTALLS += target

unix:target.path = ../lib
unix:target.path = ../lib
unix:INSTALLS += target

DESTDIR = .
