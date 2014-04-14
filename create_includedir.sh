#!/bin/bash

####################################################################
## create_includedir: a script to an include directory populated
## symbolic links to CHEF source directories. This allows CHEF
## code to access include files in other source trees during build.
## 
## Extracted-from: bootstrap
## Author: Jean-Francois Ostiguy
##         Fermilab
##         ostiguy@fnal.gov
##
########################################################################

set -x

##----------------------------------------
## create links for include subdirectories
##----------------------------------------


echo "Creating header file links ..."

if [ ! -d ./include ]
then mkdir include
fi

cd ./include

srcdirs="basic_toolkit beamline bmlfactory mxyzptlk"
srcdirs="${srcdirs} physics_toolkit integrator gms python-bindings"

for dir in ${srcdirs}
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
