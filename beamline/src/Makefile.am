
SUBDIRS = physics

lib_LTLIBRARIES		= libbeamline.la
include source_files

libbeamline_la_LDFLAGS = physics/6d/*.lo physics/mad/*.lo

CPPFLAGS = -I/usr/local/include -I$(BOOST_INC) -I$(FFTW3_INC) -I$(top_srcdir)/../include 


