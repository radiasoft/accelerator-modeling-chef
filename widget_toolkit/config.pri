###################################################################
#
# config.pro.in
#
# Author: Jean-Francois Ostiguy
#         Accelerator Division
#         Fermi National Accelerator Laboratory 
#         ostiguy@fnal.gov
#
########################################################################

   unix:CHEF_INSTALL_TOP_DIR   =  /home/ostiguy/CHEF/chef-tmp  
   unix:FNAL_INSTALL_TOP_DIR   =  /home/ostiguy/CHEF/chef-tmp
   unix:BOOST_INSTALL_TOP_DIR  =  /usr
   unix:BOOST_VERSION          =  1_32
   unix:GLIB_INSTALL_TOP_DIR   =  /usr
   unix:GLIB_VERSION           =  2.0
   unix:PYTHON_INSTALL_TOP_DIR =  /usr
   unix:PYTHON_VERSION         =  2.3
   unix:QWT_INSTALL_TOP_DIR    =  /usr/qt/3/include
   unix:QUTEXR_INSTALL_TOP_DIR =  /usr/local

   windows:CHEF_INSTALL_TOP_DIR   =  /root/chef-tmp  
   windows:FNAL_INSTALL_TOP_DIR   =  /root/chef-tmp
   windows:BOOST_INSTALL_TOP_DIR  =  /usr
   windows:BOOST_VERSION          =  1_32
   windows:GLIB_INSTALL_TOP_DIR   =  /usr/include
   windows:GLIB_VERSION           =  2.0
   windows:PYTHON_INSTALL_TOP_DIR =  /usr
   windows:PYTHON_VERSION         =  2.3
   windows:QUTEXR_INSTALL_TOP_DIR =  /usr/local/include

########################################################################

unix:BOOST_INC  =  $$join(BOOST_INSTALL_TOP_DIR, "/include/boost-" , ,      /include/boost-        )
unix:BOOST_INC  =  $$join(BOOST_INC,             $$BOOST_VERSION , ,        $$BOOST_VERSION        )


unix:GLIB_INC  =  $$join(GLIB_INSTALL_TOP_DIR,   "/include/glib-" , ,      /include/glib-         )
unix:GLIB_INC  =  $$join(GLIB_INC,               $$GLIB_VERSION , ,        $$GLIB_VERSION         )


unix:PYTHON_INC  =  $$join(PYTHON_INSTALL_TOP_DIR, "/include/python" , ,   /include/python        )
unix:PYTHON_INC  =  $$join(PYTHON_INC,             $$PYTHON_VERSION , ,    $$PYTHON_VERSION       )


unix:GLIBCONFIG_INC  =  $$join(GLIB_INSTALL_TOP_DIR,   "/lib/glib-" , ,          /lib/glib-         )
unix:GLIBCONFIG_INC  =  $$join(GLIBCONFIG_INC,         $$GLIBCONFIG_VERSION , ,  $$GLIB_VERSION     )
unix:GLIBCONFIG_INC  =  $$join(GLIBCONFIG_INC,         /include, ,               /include           )

unix:QUTEXR_INC  =  $$join(QUTEXR_INSTALL_TOP_DIR,  "/include/qutexr" , ,        /include/qutexr    )

unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/beamline" , ,      /include/beamline        )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/bmlfactory", ,     /include/bmlfactory      )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/mxyzptlk", ,       /include/mxyzptlk        )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/basic_toolkit", ,  /include/basic_toolkit   )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/physics_toolkit", ,/include/physics_toolkit )
unix:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR, "/include/gms", ,            /include/gms)             
unix:INCLUDEPATH += $$BOOST_INC
unix:INCLUDEPATH += $$GLIB_INC $$GLIBCONFIG_INC
unix:INCLUDEPATH += $$QUTEXR_INC 
unix:INCLUDEPATH += ./include
unix:INCLUDEPATH += ./src/ui

windows:INCLUDEPATH += $$join(BOOST_INSTALL_TOP_DIR, "\include\boost-1_32",     , \include\boost-1_32      ) 
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,  "\include\beamline"  ,     , \include\beamline        )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\bmlfactory",     , \include\bmlfactory      )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\mxyzptlk",       , \include\mxyzptlk        )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\basic_toolkit",  , \include\basic_toolkit   )
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR   "\include\physics_toolkit" , \include\physics_toolkit )
#
# for block allocator
#
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"\include\gms",                 , \include\gms             )             
windows:INCLUDEPATH += $$join(FNAL_INSTALL_TOP_DIR,"\include",                     , \include                 )             

#############################################################################
 
unix:BOOST_LIBDIR   =    $$join(BOOST_INSTALL_TOP_DIR,   "/lib" , ,     /lib        )
unix:GLIB_LIBDIR    =    $$join(GLIB_INSTALL_TOP_DIR,    "/lib" , ,     /lib        )
unix:FNAL_LIBDIR    =    $$join(FNAL_INSTALL_TOP_DIR,    "/lib" , ,     /lib        )
unix:CHEF_LIBDIR    =    $$join(CHEF_INSTALL_TOP_DIR,    "/lib" , ,     /lib        )
unix:QUTEXR_LIBDIR  =    $$join(QUTEXR_INSTALL_TOPDIR,   "/lib" , ,     /lib        )


################################################################################

QMAKE_CXXFLAGS           += -pipe 

   unix:target.path = $$join(CHEF_INSTALL_TOP_DIR, "/lib", , /lib)
windows:target.path = $$join(CHEF_INSTALL_TOP_DIR, "\lib", , \lib)


windows:UI_DIR       =  src\ui
windows:MOC_DIR      =  src\moc
windows:OBJECTS_DIR  =  src\obj

unix:UI_DIR       =  src/ui
unix:MOC_DIR      =  src/moc
unix:OBJECTS_DIR  =  src/obj





