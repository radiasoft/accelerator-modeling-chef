########################################################################
# 
# FILE:          fncdata.pro
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

INCLUDEPATH     += ../plotting/chefplot/include
INCLUDEPATH     += ../plotting/chefplot/src/ui

HEADERS += ./include/MomentsFncData.h \
           ./include/DspnFncData.h \
           ./include/ETFncData.h \
           ./include/LBFncData.h \
           ./include/LattFncData.h

SOURCES += ./src/MomentsFncData.cc \
           ./src/DspnFncData.cc \
           ./src/ETFncData.cc \
           ./src/LBFncData.cc \
           ./src/LattFncData.cc
