###################################################################
#
# tunes.pro
#
# Author: Jean-Francois Ostiguy/ APC
#         ostiguy@fnal.gov
#         Fermi National Accelerator Laboratory 
#
########################################################################

CONFIG += qt warn_on debug thread rtti exceptions dll

TEMPLATE	= lib 
LANGUAGE	= C++

!include ( ../../../chef-config/config.pri ) {
        message( "Error: failed to find global configuration file config.pri" ) 
}

HEADERS	+= include/TuneDiagram.h
SOURCES	+= src/TuneDiagram.cc
SOURCES	+= src/ResonanceLine.cc
FORMS	+= src/TuneDiagramBase.ui

unix:LIBS  +=  -Wl,--rpath,$${FNAL_LIBDIR} -Wl,--rpath,$${GLIB_LIBDIR}
