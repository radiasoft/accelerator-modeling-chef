lib_LTLIBRARIES		= libbeamline.la
include source_files

CPPFLAGS = $(BOOST_INC) $(FFTW3_INC) -I$(top_srcdir)/../include 

if IMPLICIT_TEMPLATES
LTCXXCOMPILE_TEMPLATES = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -g  -O2

CXXCOMPILE_TEMPLATES = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CXXFLAGS) -g -O2

else

LTCXXCOMPILE_TEMPLATES = $(LIBTOOL) --tag=CXX $(AM_LIBTOOLFLAGS) $(LIBTOOLFLAGS) \
	--mode=compile $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) -DBASICTOOLKIT_EXPLICIT_TEMPLATES -DMXYZPTLK_EXPLICIT_TEMPLATES -DBEAMLINE_EXPLICIT_TEMPLATES  -g  -O2

CXXCOMPILE_TEMPLATES = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
	$(AM_CPPFLAGS) $(CPPFLAGS) -DBASICTOOLKIT_EXPLICIT_TEMPLATES  -DMXYZPTLK_EXPLICIT_TEMPLATES -DBEAMLINE_EXPLICIT_TEMPLATES -g -O2

endif

TemplateInstantiations.o : TemplateInstantiations.cc
	$(CXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
#	source='$<' object='$@' libtool=no \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(CXXCOMPILE) -c -o $@ $<

TemplateInstantiations.lo: TemplateInstantiations.cc
	$(LTCXXCOMPILE_TEMPLATES) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Plo
#	source='$<' object='$@' libtool=yes \
#	DEPDIR=$(DEPDIR) $(CXXDEPMODE) $(depcomp) \
#	$(LTCXXCOMPILE) -c -o $@ $<

