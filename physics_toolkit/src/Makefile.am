#
# Makefile.am for physics_toolkit
# Author: ostiguy@fnal.gov
#

lib_LTLIBRARIES	= libphysics_toolkit.la
libphysics_toolkit_la_LDFLAGS = -lboost_filesystem-mt -lboost_system-mt

include source_files

AM_CPPFLAGS = $(BOOST_INC) -I/usr/local/vsqlite++/include -I$(top_srcdir)/../include
