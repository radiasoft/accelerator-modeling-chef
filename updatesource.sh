#!/bin/sh

#-----------------------------------------------------------------------
# updatesource.sh
#
# Author: ostiguy@fnal.gov
#
# Updates a list of source files (to be included in Makefile.am)  
#-----------------------------------------------------------------------

more source_files | tr " " "\n" > test.source_files.old

current_dir=`pwd`

while [ ! -d `pwd`/include ]
do 
  cd ..
done

libname=`basename \`pwd\``
export libname

cd $current_dir


#----------------------------------------------------------
# note: yacc sources must be listed *before* lex sources !
#----------------------------------------------------------

echo lib$libname"_la_SOURCES"=*.{cpp,cxx,cc,c,y,l,C} > source_files
unset libname
more source_files | tr " " "\n" > test.source_files.new
diff test.source_files.old test.source_files.new | sed "s/</Removed/g" | sed "s/>/Added/g"
rm test.source_files.old
rm test.source_files.new
