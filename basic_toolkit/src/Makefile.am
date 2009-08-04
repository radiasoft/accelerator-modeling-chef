##################################################################################
##################################################################################
####
####  File: basic_toolkit/src/Makefile.am 
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

lib_LTLIBRARIES		= libbasic_toolkit.la
include source_files

AM_CXXFLAGS		     =  ${OPTFLAGS} $(TEMPLATEFLAGS) 
AM_CFLAGS		     =  ${OPTFLAGS}  
AM_CPPFLAGS		     =  ${LOCALDEFS} ${VSQLITEPP_INC} ${SQLITE3_INC} ${BOOST_INC} ${FFTW3_INC} -I$(top_srcdir)/../include 
libbasic_toolkit_la_LDFLAGS  =  $(VSQLITEPP_LIB) -lvsqlite++ $(SQLITE3_LIB) -lsqlite3  $(FFTW3_LIB) -lfftw3 

if !IMPLICIT_TEMPLATES

LTCXXCOMPILE_TEMPLATES = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) ${OPTFLAGS}  $(CXXFLAGS)

CXXCOMPILE_TEMPLATES = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(OPTFLAGS) $(CXXFLAGS)

else

LTCXXCOMPILE_TEMPLATES = $(LTCXXCOMPILE)
CXXCOMPILE_TEMPLATES   = $(CXXCOMPILE)

endif

TemplateInstantiations.o : TemplateInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo  -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE_TEMPLATES) -c -o $@ $<

TemplateInstantiations.lo: TemplateInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE_TEMPLATES) -c -o $@ $<

IntArrayInstantiations.o : IntArrayInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
# 	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
# 	$(CXXCOMPILE_TEMPLATES) -c -o $@ $<

IntArrayInstantiations.lo: IntArrayInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE_TEMPLATES) -c -o $@ $<

TMatrixInstantiations.o : TMatrixInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE_TEMPLATES) -c -o $@ $<

TMatrixInstantiations.lo: TMatrixInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE_TEMPLATES) -c -o $@ $<

