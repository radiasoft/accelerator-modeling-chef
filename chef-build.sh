#!/usr/bin/env bash

######################################################
# chef-build.sh 
#
# A simple script to automate the CHEF daily build
# from CVS
#
#
# Jean-Francois Ostiguy
# ostiguy@fnal.gov
#
######################################################

set +x

MAKE=gmake

help_build() {

  echo "Usage: $0 qtdir_directory qmakespec" 
  echo "Options:"
  echo "-I installation_directory"
  exit 1
}

build_all() {
./bootstrap

mkdir BUILD
cd BUILD
../configure --prefix=$TOPDIR/RUNTIME
if [ $? -ne 0 ]; then return; fi
$MAKE
if [ $? -ne 0 ]; then return; fi
$MAKE install
if [ $? -ne 0 ]; then return; fi
cd ..
cd widget_toolkit
qmake widget_toolkit.pro
if [ $? -ne 0 ];  then exit; fi
$MAKE
if [ $? -ne 0 ];  then exit; fi
$MAKE install
if [ $? -ne 0 ];  then exit; fi
cd ..
cd chef-mdi
qmake chef.pro
if [ $? -ne 0 ];  then exit; fi
$MAKE
if [ $? -ne 0 ];  then exit; fi
}

INSTALLDIR=""
#
if [ $# -lt 2 ]; then
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
  echo 'File  config.pri.'$nodename 'not found.'
  exit 1
fi
cd ..
#
while getopts "?I:" opt
do
   case "$opt" in 
      I)  INSTALLDIR="$OPTARG";;
      \?) help_build;;
   esac
done
#
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
PATH=/usr/local/ap/bin\:/usr/local/bin\:$QTDIR/bin\:$PATH
export PATH
#
QMAKESPEC=$2 
export QMAKESPEC
#
SED=/bin/sed 
export SED
#
echo 'CHEF BUILD started on node' $nodename
echo `date`

build_all

if [ $? -eq 0 ]
then
  echo '--------CHEF BUILD SUCCESSFUL ---------'
  echo `date`
else
  echo '--------CHEF BUILD FAILED ---------'
  echo `date`
fi
