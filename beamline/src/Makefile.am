
SUBDIRS = physics

lib_LTLIBRARIES		= libbeamline.la
include source_files

libbeamline_la_LDFLAGS = physics/6d/*.lo physics/mad/*.lo

CPPFLAGS = -I$(BOOST_INC) -I$(top_srcdir)/../include 

EXTRA_DIST = CF_rbendConstructor.ins  CF_sbendConstructor.ins
