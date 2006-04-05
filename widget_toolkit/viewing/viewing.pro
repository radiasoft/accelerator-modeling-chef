########################################################################
# 
# FILE:          viewing.pro 
#
# AUTHOR:        Leo Michelotti
#                following the pattern set by Jean-Francois Ostiguy
#                
#                Accelerator Division
#                Fermi National Accelerator Laboratory 
#                michelotti@fnal.gov
#       
#######################################################################


include ( ../../chef-config/config.pri )

TEMPLATE        = lib
LANGUAGE        = C++

windows:CONFIG  += qt opengl x11 debug warn_on thread staticlib exceptions rtti
unix:CONFIG     += qt opengl x11 warn_on debug thread dll       rtti exceptions

HEADERS += ./include/BeamlineBrowser.h \
           ./include/EditDialog.h \
           ./include/SiteViewer.h

SOURCES += ./src/BeamlineBrowser.cc \
           ./src/EditDialog.cc \
           ./src/SiteViewer.cc
