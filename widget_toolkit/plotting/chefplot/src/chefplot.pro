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

########################################################################
# THERE SHOULD BE NO NEED TO CHANGE ANYTHING BELOW THIS LINE !
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++


unix:CONFIG	+= qt warn_on debug thread rtti exceptions dll
windows:CONFIG	+= qt warn_on       thread rtti exceptions staticlib

unix:LIBS	+= -lqwt -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -lglib
###unix:LIBS	+= -lqwt -lbmlfactory -lphysics_toolkit -lbeamline -lmxyzptlk -lbasic_toolkit -lglib-2.0



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

unix:INCLUDEPATH += ../include
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/beamline" , ,      /include/beamline        )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/bmlfactory", ,     /include/bmlfactory      )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/mxyzptlk", ,       /include/mxyzptlk        )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/basic_toolkit", ,  /include/basic_toolkit   )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/physics_toolkit", ,/include/physics_toolkit )
### for block allocator
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"/include/gms", ,             /include/gms)             
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"/include", ,                 /include)             


windows:INCLUDEPATH += ..\include
windows:INCLUDEPATH += $$join(BOOST_INSTALL_TOP_DIR, "\include\boost-1_32",     , \include\boost-1_32      ) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,  "\include\beamline"  ,     , \include\beamline        )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\bmlfactory",     , \include\bmlfactory      )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\mxyzptlk",       , \include\mxyzptlk        )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\basic_toolkit",  , \include\basic_toolkit   )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\physics_toolkit" , \include\physics_toolkit )
### for block allocator
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"\include\gms",                 , \include\gms             )             
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"\include",                     , \include                 )             


   unix:target.path = $$join(CHEF_INSTALL_TOP_DIR, "/lib", , /lib)
windows:target.path = $$join(CHEF_INSTALL_TOP_DIR, "\lib", , \lib)

INSTALLS += target

DESTDIR = ../lib

unix:QMAKE_LIBDIR_FLAGS += -L/opt/chef/lib 
unix:QMAKE_LIBDIR_FLAGS += -L/usr/local/lib 
unix:QMAKE_LIBDIR_FLAGS += -L/usr/local/ap/lib 
unix:QMAKE_LIBDIR_FLAGS += -L/usr/local/ap/FNAL/lib 

unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/ap/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/ap/FNAL/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/opt/chef/lib 
unix:QMAKE_LFLAGS_SHAPP += -Wl,-rpath,/usr/local/fnal-1st-order-proto-3/lib 

windows:UI_DIR       =  .\ui
windows:MOC_DIR      =  .\moc
windows:OBJECTS_DIR  =  .\obj

unix:UI_DIR       =  ./ui
unix:MOC_DIR      =  ./moc
unix:OBJECTS_DIR  =  ./obj


QMAKE_CXXFLAGS_MT        += -pipe 
QMAKE_CXXFLAGS_MT_DBG    += -pipe 
QMAKE_CXXFLAGS_MT_DLL    += -pipe 
QMAKE_CXXFLAGS_MT_DLLDBG += -pipe 
QMAKE_CXXFLAGS_RELEASE   += -pipe 
QMAKE_CXXFLAGS_SHLIB     += -pipe 
