###################################################################
#
# tunespace.pro
#
# Author: Leo Michelotti
#         Computing Division/AMR
#         Fermi National Accelerator Laboratory 
#         michelotti@fnal.gov
#
########################################################################

TEMPLATE	= lib
LANGUAGE	= C++

#
# Note: the relative path is broken under UNIX. Forward slashes do not seem to work, but
# win32 style backslashes are OK.
#
!include ( ../../../chef-config/config.pri ) {
        message( "Error: failed to find global configuration file config.pri" ) 
}

unix:CONFIG += qt warn_on debug thread rtti exceptions dll

HEADERS	+= include/TuneDiagram.h
SOURCES	+= src/TuneDiagram.cc

unix:LIBS  +=  -Wl,--rpath,$${FNAL_LIBDIR} -Wl,--rpath,$${GLIB_LIBDIR}
