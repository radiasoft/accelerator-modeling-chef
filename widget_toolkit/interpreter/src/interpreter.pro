########################################################################
# 
# FILE:          interpreter.pro 
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
   unix:BOOST_VERSION         = 1_32
windows:BOOST_VERSION         = 1_32

   unix:PYTHON_INSTALL_TOP_DIR =  /usr/local/ap/
   unix:PYTHON_VERSION         =  2.3 

windows:PYTHON_INSTALL_TOP_DIR =  c:\python
windows:PYTHON_VERSION         =  2.3


   unix:GLIB_INSTALL_TOP_DIR =  /usr/local/ap
   unix:GLIB_VERSION         =  2.0


windows:GLIB_INSTALL_TOP_DIR =  c:\python
windows:GLIB_VERSION         =  2.0


   unix:QUTEXR_INSTALL_TOP_DIR =  /usr/local/ap/qt3/include
windows:QUTEXR_INSTALL_TOP_DIR =  c:\Qt\3.3.3\include 

########################################################################
# THERE SHOULD BE NO NEED TO CHANGE ANYTHING BELOW THIS LINE !
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++

   unix:CONFIG	+= qt warn_on debug thread rtti exception staticlib
windows:CONFIG	+= qt warn_on debug thread rtti exceptions staticlib

BOOST_INCLUDE    = $$join(BOOST_INSTALL_TOP_DIR, "/include",, "/include")   
BOOST_INCLUDE    = $$join(BOOST_INCLUDE, "/boost-",, "/boost-")   
BOOST_INCLUDE    = $$join(BOOST_INCLUDE, $$BOOST_VERSION,, $$BOOST_VERSION)   

PYTHON_INCLUDE   = $$join(PYTHON_INSTALL_TOP_DIR, "/include",, /include)
PYTHON_INCLUDE   = $$join(PYTHON_INCLUDE, "/python",, /python)
PYTHON_INCLUDE   = $$join(PYTHON_INCLUDE, $$PYTHON_VERSION,, $$PYTHON_VERSION)   

GLIBCONFIG_INCLUDE = $$join(GLIB_INSTALL_TOP_DIR, /lib,, /lib) 
GLIBCONFIG_INCLUDE = $$join(GLIBCONFIG_INCLUDE,   /glib-,, /glib-)
GLIBCONFIG_INCLUDE = $$join(GLIBCONFIG_INCLUDE,   $$GLIB_VERSION,, $$GLIB_VERSION)
GLIBCONFIG_INCLUDE = $$join(GLIBCONFIG_INCLUDE,   /include,, /include)

unix:INCLUDEPATH += . ../include 
unix:INCLUDEPATH += $$BOOST_INCLUDE 
unix:INCLUDEPATH += $$PYTHON_INCLUDE
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/basic_toolkit,,   /include/basic_toolkit) 
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/mxyzptlk,,        /include/mxyzptlk) 
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/beamline,,        /include/beamline) 
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/physics_toolkit,, /include/physics_toolkit) 
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/bmlfactory,,      /include/bmlfactory) 
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include/gms,,             /include/gms)
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, /include,,    /include)
unix:INCLUDEPATH += $$join(GLIB_INSTALL_TOP_DIR, /include,, /include) 
unix:INCLUDEPATH += $$GLIBCONFIG_INCLUDE

windows:INCLUDEPATH += . ../include 
windows:INCLUDEPATH += $$BOOST_INCLUDE
windows:INCLUDEPATH += $$PYTHON_INCLUDE 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\basic_toolkit,,   \include\basic_toolkit) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\mxyzptlk,,        \include\mxyzptlk) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\beamline,,        \include\beamline) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\physics_toolkit,, \include\physics_toolkit) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\bmlfactory,,      \include\bmlfactory) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include\gms,,             \include\gms)
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, \include,,    \include)
windows:INCLUDEPATH += $$join(GLIB_INSTALL_TOP_DIR, /include,, /include) 
windows:INCLUDEPATH += $$GLIBCONFIG_INCLUDE


unix:LIBS += -L/opt/chef/lib -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
unix:LIBS += -lqwt 
unix:LIBS += /usr/local/lib/python2.3/config/libpython2.3.a -L/usr/local/lib -lboost_python-gcc-mt-1_32 
unix:LIBS += -L/opt/chef/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 

windows:LIBS += -L/opt/chef/lib -lpychefplot -lpybmlfactory -lpybeamline -lpyphysics_toolkit -lpymxyzptlk -lpybasic_toolkit 
windows:LIBS += -lqwt 
windows:LIBS += /usr/local/lib/python2.3/config/libpython2.3.a -L/usr/local/lib -lboost_python-gcc-mt-1_32 
windows:LIBS += -L/opt/chef/lib -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit 



HEADERS	+= ../include/ioredirector.h \
 	../include/qpychef.h \
        ../include/interpreter.h

SOURCES	+= qpychef.cpp \
           interpreter.cpp \
           ioredirector.cpp \
           py-ioredirector.cpp \
	 ./python-specific/2.3/testcomplete.c 

FORMS	= qpychefbase.ui

unix {
  UI_DIR =      ui
  MOC_DIR =     moc
  OBJECTS_DIR = obj
}

windows{

  UI_DIR      = .\ui
  MOC_DIR     = .\moc
  OBJECTS_DIR = .\obj
}

unix:QMAKE_LFLAGS_DEBUG     +=-Wl,--export-dynamic  -Wl,-rpath,/home/Administrator/chef-win32/lib -Wl,-rpath,/usr/local/lib  
unix:QMAKE_LFLAGS_RELEASE   +=-Wl,--export-dynamic  -Wl,-rpath,/home/Administrator/chef-win32/lib -Wl,-rpath,/usr/local/lib


target.path = /home/Administrator/chef-win32/lib
INSTALLS += target

DESTDIR = ../lib
