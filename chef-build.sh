#!/usr/bin/env bash

######################################################
# chef-build.sh 
#
# Aa simple script to automate the CHEF daily build
#
# Jean-Francois Ostiguy
# ostiguy@fnal.gov
#
######################################################

MAKE=gmake



help_build() {

  echo "Usage: $0 qtdir_directory" 
  echo "Options:"
  echo "-I installation_directory"
  exit 1
}

INSTALLDIR=""
#
if [ $# -lt 1 ]; then
  help_build
fi

echo `uname -a` | awk '{ print $2 }' - > tmp_node_name
nodename=`cat tmp_node_name`
nodename=${nodename%%.*}
\rm tmp_node_name

cd chef-config
if [ -L config.pri ]
then
 \rm config.pri
fi

if [ -a config.pri.$nodename ]
then 
  ln -s config.pri.$nodename  config.pri
else
  echo 'File ' config.pri.$nodename 'not found.'
  exit 1
fi
cd ..

while getopts I: opt
do
   case "$opt" in 
      I)  INSTALLDIR="$OPTARG";;
      \?) help_build;;
   esac
done

TOPDIR=`pwd`
export TOPDIR
#
if [ -z $INSTALLDIR ]
then
   INSTALLDIR=$TOPDIR/RUNTIME
fi
export INSTALLDIR 
#
QTDIR=$1
export QTDIR
#
PATH=/usr/local/ap/bin\:/usr/local/bin:\$QTDIR/bin\:$PATH
export PATH
#
SED=/bin/sed 
export SED
#
echo 'CHEF BUILD started on node $nodename'
echo `date`

./bootstrap
#
mkdir BUILD
cd BUILD
../configure --prefix=$TOPDIR/RUNTIME
#
$MAKE
$MAKE install
cd ..
cd widget_toolkit
qmake widget_toolkit.pro
$MAKE
$MAKE install
cd ..
cd chef-mdi
qmake chef.pro
$MAKE
echo '--------CHEF BUILD SUCCESSFUL ---------'
echo `date`
