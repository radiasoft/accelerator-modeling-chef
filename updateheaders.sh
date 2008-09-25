#!/bin/sh

#-----------------------------------------------------------------------
# updateheaders.sh
#
# Author: ostiguy@fnal.gov
#
# Updates a list of header files (to be included in Makefile.am)  
#-----------------------------------------------------------------------

more header_files | tr " " "\n" > test.header_files.old
headername=`basename \`pwd\``
echo includedir = \$\{prefix\}/include/$headername > header_files
echo include_HEADERS = include/*.{h,icc,tcc} >> header_files
more header_files | tr " " "\n" > test.header_files.new
diff test.header_files.old test.header_files.new | sed "s/</Removed/g" | sed "s/>/Added/g"
rm test.header_files.new
rm test.header_files.old
