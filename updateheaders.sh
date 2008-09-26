#!/bin/sh

#-----------------------------------------------------------------------
# updateheaders.sh
#
# Author: ostiguy@fnal.gov
#
# Updates a list of header files (to be included in Makefile.am)  
#-----------------------------------------------------------------------

if [ -f header_files ]; then  mv header_files  header_files.old; else touch header_files.old; fi
headername=`basename \`pwd\``
echo  includedir = \$\{prefix\}/include/$headername > header_files.new
echo "include_HEADERS = " | tr -d "\n"  >> header_files.new 
ls include/ | grep '.*\.h$'    | sed -e '/[a-zA-Z\.0-9_]*/i\ include/' | tr -d "\n" >> header_files.new  
ls include/ | grep '.*\.hpp$'  | sed -e '/[a-zA-Z\.0-9_]*/i\ include/' | tr -d "\n" >> header_files.new   
ls include/ | grep '.*\.tcc$'  | sed -e '/[a-zA-Z\.0-9_]*/i\ include/' | tr -d "\n" >> header_files.new   
echo >>  header_files.new
diff header_files.old header_files.new | sed "s/</Removed/g" | sed "s/>/Added/g"
if [ -f header_files.new ]; then mv header_files.new header_files; fi
if [ -f header_files.old ]; then rm header_files.old; fi
