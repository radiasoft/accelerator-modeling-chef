##################################################################################
##################################################################################
####
####  File: parsers/xsif/src/Makefile.am 
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

lib_LTLIBRARIES                   =    libxsifparser.la  

libxsifparser_la_SOURCES          =    XsifParser_ypp.cc lex.xsif_yy.cc Expression.cc XSIFFactory.cc XsifParserDriver.cc 

AM_CPPFLAGS                       =    $(LOCALDEFS) $(BOOST_INC) -I. -I$(top_srcdir)/include  -I$(top_srcdir)/include/tree-container -I$(top_srcdir)/../../include  
AM_CXXFLAGS                       =    $(OPTFLAGS) 

libxsifparser_la_LDFLAGS          =  -lboost_filesystem-mt  -lboost_system-mt

LEX  = flex
YACC = bison 

XsifParser_ypp.hh: XsifParser_ypp.cc

lex.xsif_yy.cc:  xsif_lex.l
	flex -o lex.xsif_yy.cc -d $^ 

XsifParser_ypp.cc: xsif_yacc.ypp
	bison -v  $^ 




