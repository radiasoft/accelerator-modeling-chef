########################################################################
# 
# FILE:          tracking.pro 
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

INCLUDEPATH += ../tracking/src/ui

HEADERS += ./include/Orbit.h \
           ./include/Tracker.h \
           ./include/DistributionWidget.h \
           ./include/PointEdit.h \
           ./include/QtMonitor.h \
           ./include/TrbWidget.h \
           ./include/RayTrace.h \
           ./include/RayTracerNew.h 

SOURCES += ./src/Orbit.cc \
           ./src/Tracker.cc \
           ./src/DistributionWidget.cc \
           ./src/PointEdit.cc \
           ./src/QtMonitor.cc \
           ./src/TrbWidget.cc \
           ./src/RayTrace.cc \
           ./src/RayTracerNew.cc

FORMS	+= ./src/RayTracerBase.ui 
