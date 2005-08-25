#!/bin/sh

########################################################
# distclean.sh 
# Cleanup the the source tree. 
#
# Jean-Francois Ostiguy
# ostiguy@fnal.gov
########################################################

set +x

#

find . -name autom4te\.cache -exec  \rm -rf {}  \; -print 
find . -name aclocal\.m4      -exec \rm     {}  \; -print 
find . -name ltmain\.sh       -exec \rm     {}  \; -print 
find . -name config\.guess    -exec \rm     {}  \; -print 
find . -name config\.sub      -exec \rm     {}  \; -print 
find . -name source_files -exec \rm {} \; -print
find . -name missing -exec \rm {} \; -print
find . -name install-sh -exec \rm {} \; -print
find . -name depcomp -exec \rm {} \; -print
find . -name header_files -exec \rm {} \; -print
find . -name config\.h\.in -exec \rm {} \; -print
find . -name Makefile\.in -exec \rm {} \; -print
find . -name Makefile -exec \rm {} \; -print
find . -name configure -exec \rm {} \; -print

find widget_toolkit -name moc_\*  -exec \rm {} \; -print 
find widget_toolkit -name \*.so  -exec \rm {}  \; -print 
find widget_toolkit -name \*.so\.\*  -exec \rm {} \; -print 
