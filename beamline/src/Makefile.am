
SUBDIRS = physics

lib_LTLIBRARIES		= libbeamline.la
include source_files

libbeamline_la_LDFLAGS = physics/6d/*.lo physics/mad/*.lo

CPPFLAGS = -I$(top_srcdir)/../include $(BOOST_INC) $(FFTW3_INC) 


