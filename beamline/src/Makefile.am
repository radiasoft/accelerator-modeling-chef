
SUBDIRS = physics

lib_LTLIBRARIES		= libbeamline.la
include source_files

libbeamline_la_LDFLAGS = physics/6d/*.lo physics/mad/*.lo

CPPFLAGS = -I$(top_srcdir)/include -I$(top_srcdir)/../mxyzptlk/include -I$(top_srcdir)/../basic_toolkit/include -I$(top_srcdir)/../physics_toolkit/include -I$(top_srcdir)/../socket/include -I@TK_INC@ -I@X_INC@

EXTRA_DIST = CF_rbendConstructor.ins  CF_sbendConstructor.ins
