#!/bin/sh

####################################################################
## bootstrap: a script to generate a build environment 
## using the GNU autotools
## 
## Author: Jean-Francois Ostiguy
##         Fermilab
##         ostiguy@fnal.gov
##
########################################################################

set -x

rootdir=`pwd`

## remove remnants of autogenerated files. These files should **never** be put in the
## repository !

find . -name autom4te\.cache -exec  \rm -rf {}  \; -print 
find . -name aclocal\.m4      -exec \rm     {}  \; -print 
find . -name ltmain\.sh       -exec \rm     {}  \; -print 
find . -name config\.guess    -exec \rm     {}  \; -print 
find . -name config\.sub      -exec \rm     {}  \; -print 

## update the list of source files and header files. 

for dir in basic_toolkit bmlfactory mxyzptlk physics_toolkit integrator gms beamline
do
  cd   $dir
  if [ ! -L $dir ]
  then ln -s include $dir
  fi
  ${rootdir}/updateheaders.sh
  cd ./src
  ${rootdir}/updatesource.sh
  cd ../..
done

for dir in python-bindings
do
  cd   $dir
  ${rootdir}/updateheaders.sh
  cd ..
done


## use autotools to generate the build environment. 
## NOTE:  command invocation order matters !

libtoolize --force --copy
aclocal -I config
automake --add-missing --foreign --copy
autoconf


for dir in basic_toolkit beamline bmlfactory parsers mxyzptlk physics_toolkit integrator gms python-bindings
do
  cd $dir
  libtoolize --force --copy
  aclocal -I config
  autoheader
  automake --add-missing --foreign --copy
  autoconf
  cd ..
done

cd parsers/xsif
libtoolize --force --copy
aclocal -I config
autoheader
automake --add-missing --foreign --copy
autoconf
cd ..

###libtoolize --force --copy
aclocal -I config
###autoheader
automake --add-missing --foreign --copy
autoconf
cd ..


##----------------------------------------
## create links for include subdirectories
##----------------------------------------


echo "Creating header file links ..."

if [ ! -d ./include ]
then mkdir include
fi

cd ./include

for dir in basic_toolkit beamline bmlfactory mxyzptlk physics_toolkit integrator gms python-bindings
do

if [ ! -L $dir ]; then
 ln -s ../$dir/include $dir
fi

done

if [ ! -d ./parsers ]; then
 mkdir parsers
fi

if [ ! -L parsers/xsif ]; then
 ln -s ../../parsers/xsif/include parsers/xsif
fi

cd ..