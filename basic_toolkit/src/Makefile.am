##################################################################################
######
######  File: basic_toolkit/src/Makefile.am 
######
######  Copyright (c) Fermi Research Alliance LLC 
######                All Rights Reserved
######
######  Usage, modification, and redistribution are subject to terms          
######  of the License supplied with this software.
######  
######  Software and documentation created under 
######  U.S. Department of Energy Contract No. DE-AC02-07CH11359 
######  The U.S. Government retains a world-wide non-exclusive, 
######  royalty-free license to publish or reproduce documentation 
######  and software for U.S. Government purposes. This software 
######  is protected under the U.S. and Foreign Copyright Laws. 
######
######                                                                
######  Author:    Jean-Francois Ostiguy                                     
######             ostiguy@fnal.gov         
######
##################################################################################

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES		= libbasic_toolkit.la
include source_files

CPPFLAGS		     =  -I/usr/local/vsqlite++/include $(BOOST_INC) $(FFTW3_INC) -I$(top_srcdir)/../include 
libbasic_toolkit_la_LDFLAGS  =  -L/usr/local/vsqlite++/lib -lvsqlite++ $(FFTW3_LIB) -lfftw3 

if IMPLICIT_TEMPLATES
LTCXXCOMPILE_TEMPLATES = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -g  -O3

CXXCOMPILE_TEMPLATES = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -g -O3

else

LTCXXCOMPILE_TEMPLATES = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -DBASICTOOLKIT_EXPLICIT_TEMPLATES -g  -O3

CXXCOMPILE_TEMPLATES = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -DBASICTOOLKIT_EXPLICIT_TEMPLATES -g -O3

endif

TemplateInstantiations.o : TemplateInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo  -fno-inline -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

TemplateInstantiations.lo: TemplateInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -fno-inline -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

IntArrayInstantiations.o : IntArrayInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

IntArrayInstantiations.lo: IntArrayInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

TMatrixInstantiations.o : TMatrixInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

TMatrixInstantiations.lo: TMatrixInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

