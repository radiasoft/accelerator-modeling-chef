#!/bin/csh

######################################################
#
# File: test_all.sh
#
# Original: 
#  version: May 29, 2008
# 
#   Author: Leo Michelotti
#         : michelotti@fnal.gov
#
######################################################

if (0 == ${?INSTALLDIR}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable INSTALLDIR has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?BOOST_INC}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable BOOST_INC has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?QTDIR}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable QTDIR has not been set."
  echo "*** ERROR ***"
  exit 1
endif

if (0 == ${?CXX}) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Environment variable CXX has not been set."
  echo "*** ERROR ***"
  exit 1
endif

echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
echo "Beginning testing"
echo "in directory `pwd`"
echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"

make -f makefile.local >& make.log
set return_status = $status
if( $return_status ) then
  echo "*** ERROR ***"
  echo "*** ERROR *** Executables failed to build."
  echo "*** ERROR *** Please see the file make.log."
  echo "*** ERROR ***"
  exit $return_status
endif

foreach w ( lpsDemo )
  if( -f $w.tag ) then
    echo "*** WARNING ***"
    echo "*** WARNING *** File $w.tag already exists."
    echo "*** WARNING *** Omitting test $w"
    echo "*** WARNING ***"
  else
    if( -e $w.sh ) then
      chmod 755 $w.sh
      echo "  "
      echo "================================"
      echo Running $w.sh
      ./$w.sh
      set return_status = $status
      if( $return_status ) then
        echo "*** ERROR ***"
        echo "*** ERROR *** File $w.sh test failed."
        echo "*** ERROR *** Returned status: $return_status"
        echo "*** ERROR ***"
        exit $return_status
      else
        echo Finished $w.sh: tests passed! > $w.tag
        echo Finished $w.sh: tests passed!
        echo "================================"
        echo "  "
      endif
    else
      echo "*** ERROR ***"
      echo "*** ERROR *** File $w.sh does not exist."
      echo "*** ERROR ***"
      exit 1
    endif
  endif
  end

echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
echo "Congratulations! All tests passed."
echo "in directory `pwd`"
echo "-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-o-"
