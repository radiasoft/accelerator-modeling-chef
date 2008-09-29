#!/bin/sh

#-----------------------------------------------------------------------
# updatesource.sh
#
# Author: ostiguy@fnal.gov
#
# Scans the contents of the (src) directory an generated a list of source 
# files in a format suitable for inclusion in Makefile.am  
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

#-----------------------------------------------------------------
# note: yacc sources must always be listed *before* lex sources !
#-----------------------------------------------------------------

echo lib$libname"_la_SOURCES= " > source_files.new
ls | grep '.*\.cpp$' >> source_files.new  
ls | grep '.*\.cxx$' >> source_files.new  
ls | grep '.*\.cc$'  >> source_files.new  
ls | grep '.*\.c$'   >> source_files.new  
ls | grep '.*\.C$'   >> source_files.new  
ls | grep '.*\.y$'   >> source_files.new  
ls | grep '.*\.l$'   >> source_files.new  

cat source_files.new | tr "\n" " " > test.source_files.new
echo >> test.source_files.new
diff test.source_files.old test.source_files.new | sed "s/</Removed/g" | sed "s/>/Added/g"
rm source_files.new
rm test.source_files.old
mv test.source_files.new source_files
unset libname
