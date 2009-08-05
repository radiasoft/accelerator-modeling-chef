#################################################################################
##################################################################################
####
####  File: physics_toolkit/src/Makefile.am 
####
####  Copyright (c) Fermi Research Alliance LLC 
####                All Rights Reserved
####
####  Usage, modification, and redistribution are subject to terms          
####  of the License supplied with this software.
####  
####  Software and documentation created under 
####  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
####  The U.S. Government retains a world-wide non-exclusive, 
####  royalty-free license to publish or reproduce documentation 
####  and software for U.S. Government purposes. This software 
####  is protected under the U.S. and Foreign Copyright Laws. 
####
####                                                                
####  Author:    Jean-Francois Ostiguy                                     
####             ostiguy@fnal.gov         
####
##################################################################################
##################################################################################


lib_LTLIBRARIES	= libphysics_toolkit.la
libphysics_toolkit_la_LDFLAGS = -lboost_filesystem-mt -lboost_system-mt

include source_files

AM_CPPFLAGS = $(LOCALDEFS) $(BOOST_INC) $(VSQLITEPP_INC) -I$(top_srcdir)/../include

#
# note - $(TEMPLATEFLAGS) is not used for now.
# templates in physics_toolkit are always instantiated implicitly
#

AM_CXXFLAGS = $(OPTFLAGS)  -I$(top_srcdir)/../include
