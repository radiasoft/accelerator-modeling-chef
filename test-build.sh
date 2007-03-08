#!/usr/bin/env bash

######################################################
#
# File: test-build.sh
#
# Tests the CHEF build from CVS
#
# Original: 
#  version: March 8, 2007
# 
#   Author: Leo Michelotti
#         : michelotti@fnal.gov
#
#  COMMENT: I intend that this script develop organically.
#
#  WARNING: Though this script is very simple, I am more
#           comfortable using C shell than bash, so
#           things may go badly wrong.
#
######################################################

#
#####################

TOPDIR=`pwd`

# CHANGE THE NEXT TWO LINES IF THE
# DIRECTORIES HAVE BEEN GIVEN OTHER NAMES.

INSTALLDIR="$TOPDIR/RUNTIME"
BUILDDIR="$TOPDIR/BUILD"

#####################
#


#
#####################

# Get the environment variable BOOST_INC
# from a Makefile in the BUILD directory.
# In this way we are assured of using the
# same one that was used in building chef's
# libraries.

if [ ! -d $BUILDDIR/basic_toolkit ]
then
  echo '*** ERROR *** '
  echo '*** ERROR *** Directory' $BUILDDIR '/basic_toolkit not found.'
  echo '*** ERROR *** Was chef-build.sh run successfully?'
  echo '*** ERROR *** '
  exit 1
else
  cd $BUILDDIR/basic_toolkit
  echo 'Entering directory' `pwd`

  zlorfik=(` grep 'BOOST_INC' Makefile `)
  BOOST_INC=${zlorfik[2]}

  echo 'Leaving directory' `pwd`
  cd $TOPDIR
fi

#####################
#


#
#####################

# Confirm that Environment variables
# are correct before continuing

echo 'Environment :'
echo 'variables   :'
echo '------------'
echo 'BOOST_INC   :' $BOOST_INC
echo 'INSTALLDIR  :' $INSTALLDIR
echo '  '
echo -n 'Correct? '

read aok
firstchar=`expr "$aok" : '\(.\).*'`

if [ $firstchar = y  -o  $firstchar = Y ]
then 
  export TOPDIR
  export INSTALLDIR 
  export BUILDDIR
  export BOOST_INC

  # Execute test-build.sh in all directories
  # containing a file by that name.

  dirlist=`find . -name test-build.sh -print`
  for x in $dirlist
  do
    thisdir=`echo $x | sed -e "s+^\./++" | sed -e "s+/test-build.sh++"`
    if [ ! $thisdir = test-build.sh ]
    then
      cd $thisdir
      echo 'Entering directory' `pwd`
      ./test-build.sh
      echo 'Leaving directory' `pwd`
      cd $TOPDIR
    fi
  done
else
  echo 'Received negative response.'
  echo 'Stopping.'
  exit 2
fi

#####################
#
