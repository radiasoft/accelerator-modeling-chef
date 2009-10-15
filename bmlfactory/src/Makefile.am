##################################################################################
##################################################################################
####
####  File: bmlfactory/src/Makefile.am 
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

ACLOCAL_AMFLAGS = -I m4

#AM_CXXFLAGS		     =  ${OPTFLAGS} $(TEMPLATEFLAGS) 
AM_CXXFLAGS		     =  ${OPTFLAGS}  
AM_CFLAGS		     =  ${OPTFLAGS}  
AM_CPPFLAGS		     =  $(GLIB_INC) $(GLIBCONFIG_INC) $(BOOST_INC) $(FFTW3_INC) -I$(top_srcdir)/../include

libbasic_toolkit_la_LDFLAGS  =  $(VSQLITEPP_LIB) -lvsqlite++ $(SQLITE3_LIB) -lsqlite3  $(FFTW3_LIB) -lfftw3 

lib_LTLIBRARIES		= libbmlfactory.la
include source_files

libbmlfactory_la_LDFLAGS          =  $(GLIB_LIB) -lglib-2.0

AM_YFLAGS = -d
