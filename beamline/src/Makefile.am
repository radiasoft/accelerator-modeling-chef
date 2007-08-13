
SUBDIRS = physics

lib_LTLIBRARIES		= libbeamline.la
include source_files

libbeamline_la_LDFLAGS = physics/6d/*.lo physics/mad/*.lo

CPPFLAGS = $(BOOST_INC) $(FFTW3_INC) -I$(top_srcdir)/../include 


